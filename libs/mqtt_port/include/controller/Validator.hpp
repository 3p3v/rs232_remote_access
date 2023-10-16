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

    class Validator
    {
    public:
        // using Executor_ptr = std::shared_ptr<Executor>;
        // template <typename E>
        // using Custom_exec_ptr = std::shared_ptr<E>;
        template <typename E>
        using Executor_ptr = std::shared_ptr<E>;
        using Base_executor_ptr = Executor_ptr<Executor>;
        using Pair_t = std::pair<std::string, Base_executor_ptr>;
        // using Cont_tr= std::unordered_map<Pair_t>;
        using Cont_t = std::unordered_map<Pair_t::first_type, Pair_t::second_type>;

    private:
        Cont_t channels = Cont_t();

    protected:
    public:
        Validator();
        bool validate(const std::string &channel_name);
        Base_executor_ptr get_exec(const std::string &channel_name);
        template <class E, typename S, typename... E_rgs>
        void add_channel(S &&channel_name, E_rgs &&...e_args)
        {
            if (channels.find(channel_name) == channels.end())
            {
                channels.insert(std::pair<Pair_t::first_type, Pair_t::second_type>(std::forward<S>(channel_name), Base_executor_ptr(new E(std::forward<E_rgs>(e_args)...))));
            }
            else
            {
                throw std::logic_error("Cannot create two instances of same channel.");
            }
        }

        std::unordered_set<std::string> get_channels();
    };

    // void Traffic_receiver::for_each_channel<Func>(Func func)
    // {

    // }
}