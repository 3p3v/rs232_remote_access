#pragma once

#include <vector>
#include <unordered_set>
#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <Executor.hpp>

namespace Mqtt_port
{
    class Traffic_receiver
    {
        using Executor_ptr = std::unique_ptr<Executor>;
        using Cont_type = std::unordered_map<std::string, Executor_ptr>;

    private:
        Cont_type channels;

    protected:
        void validate(std::string channel_name);
        void read(const std::string &channel_name, const Executor::Data &data);

    public:
        template<typename S, class E>
        void add_channel(S&& channel_name, E&& executor)
        {
            if(!channels.insert(std::make_pair(std::forward<S>(channel_name), std::make_unique<Executor>(std::forward<E>(executor)))))
                throw std::logic_error("Cannot create two instances of same channel.");
        }

        std::unordered_set<std::string> get_channels();
    };

    // void Traffic_receiver::for_each_channel<Func>(Func func)
    // {

    // }
}