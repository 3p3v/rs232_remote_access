#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <Executor.hpp>

namespace Mqtt_port
{
    class Traffic_receiver
    {
        using Cont_type = std::unordered_map<std::string, std::unique_ptr<Executor>>;

    private:
        Cont_type channels;
    
    protected:
        void validate(std::string channel_name);
        void pass(std::string channel_name, const Executor::Data_type &data);

    public:
        template<typename S, class E>
        void add_channel(S channel_name, E && executor)
        {
            if(!channels.insert(std::make_pair(std::forward<S>(channel_name), std::make_unique<Executor>(std::forward<E>(executor)))))
                throw std::logic_error("Cannot create two instances of same channel.");
        }
    };
}