#pragma once

#include <Controller.hpp>
#include <Server.hpp>
#include <User.hpp>
#include <Impl_adder_ref.hpp>
#include <optional>
#include <thread>
#include <future>

namespace Impl
{
    /// @brief Start underlying low-level drivers
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

        /// @brief Get initialized device adder
        /// @return Device adder
        Impl_adder_ref get_adder() &;

        /// @brief Run underlying drivers, connect to broker
        /// @return Pair for optional error thrown by the serial driver and thread running the driver
        Future_and_thread connect() &;
    };

    template <typename... Args_t>
    inline Impl_starter::Impl_starter(Args_t &&...args)
        : controller{std::forward<Args_t>(args)...}
    {
    }

}