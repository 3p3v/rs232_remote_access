#include <Impl_starter.hpp>
#include <Serial_context.hpp>
#include <Device_init.hpp>
#include <Mqtt_impl.hpp>
#include <Serial_impl.hpp>
#include <thread>
#include <optional>

Base_device_init Impl::Impl_starter::get_device_factory() &
{
    return Impl::Device_init{
        Mqtt_impl_factory{controller},
        Serial_impl_factory{}
    }
}

Impl::Impl_starter::Future_and_thread Impl::Impl_starter::connect() &
{
    /* Connect to broker */
    controller.connect(
        []() {

        },
        [](int code)
        {
            throw std::runtime_error{"Disconnected from server, code: " + std::to_string(code) + "!"};
        },
        [](int code)
        {
            throw std::runtime_error{"Could not connect to server, code: " + std::to_string(code) + "!"};
        });

    while (!controller.is_connected()); // TODO wait for connection asynchroniously

    /* Run serial context */
    std::promise<std::optional<std::exception>> p;
    auto f = p.get_future();
    auto serial_thread = std::thread{
        [](std::promise<std::optional<std::exception>> &&p)
        {  
            try
            {
                /* Run underlying Boost context for serial ports */
                Phy_serial::Serial_context::run();
            }
            catch(const std::exception& e)
            {
                p.set_value(e);
            }
        },
        std::move(p)
    };

    return std::make_pair(std::move(f), std::move(serial_thread));
}
