#include <Impl_starter.hpp>
#include <Serial_context.hpp>
#include <thread>
#include <optional>

Impl::Impl_adder_ref Impl::Impl_starter::get_adder() &
{
    return Impl_adder_ref{controller};
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
        []()
        {
            std::optional<std::exception> except;
            
            try
            {
                /* Run underlying Boost context for serial ports */
                Phy_serial::Serial_context::run();
            }
            catch(const std::exception& e)
            {
                except = e;
            }

            return except;
        },
        std::move(p)
    };

    return std::make_pair(std::move(f), std::move(serial_thread));
}
