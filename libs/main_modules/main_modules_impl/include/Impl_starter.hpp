#pragma once

#include <Controller.hpp>
#include <Server.hpp>
#include <User.hpp>
#include <Impl_adder_ref.hpp>
#include <thread>
#include <future>

namespace Impl
{
    class Impl_starter final
    {
    public:
        using Future = std::future<std::optional<std::exception>>;
        using Future_and_thread = std::pair<Future, std::thread>;

    private:
        Mqtt_port::Impl::Controller controller;

    public:
        /// @brief Construct object, initialize MQTT controller
        /// @tparam ...Args_t
        /// @param ...args Arguments passed to controller
        template <typename... Args_t>
        Impl_starter(Args_t &&...args);

        Impl_adder_ref get_adder() &;
        Future_and_thread connect() &;
    };

    template <typename... Args_t>
    inline Impl_starter::Impl_starter(Args_t &&...args)
        : controller{std::forward<Args_t>(args)...}
    {
    }

}