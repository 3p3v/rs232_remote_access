#pragma once

#include <Base_device_init.hpp>
#include <Exception_handler.hpp>
#include <memory>

using namespace Logic;

namespace Impl
{
    /// @brief Contains all nessesary components to run the application (device containers, info containers, debug, error handling, ect.)
    class Collective_general final
    {
        /// @brief Init of the devices
        std::unique_ptr<Base_device_init> device_init; 

        Exception_handler exception_handler;

    public:
        /// @brief 
        /// @param adder 
        /// @param close_on_timeout 
        /// @param close_on_data_loss 
        /// @param close_on_protocol_error 
        /// @param debug 
        Collective_general(
            std::unique_ptr<Base_device_init> &&device_init);
            // bool close_on_timeout,
            // bool close_on_data_loss,
            // bool close_on_protocol_error,
            // bool debug);

        Collective_general(Collective_general &&) = delete;
        Collective_general& operator=(Collective_general &&) = delete;
        Collective_general(const Collective_general &) = delete;
        Collective_general& operator=(const Collective_general &) = delete;
        ~Collective_general() = default;

        /// @brief Add devices from some container
        /// @tparam Iter_t 
        /// @param begin 
        /// @param end 
        /// @return 
        template <typename Iter_t>
        void add_devices(Iter_t begin, Iter_t end) &;
    };

    template <typename Iter_t>
    inline void Collective_general::add_devices(Iter_t begin, Iter_t end) &
    {   
        std::for_each(
            begin,
            end,
            [&, this](auto &dev)
            {
                adder->add(std::move(dev.sett), dev.write_access, std::move(dev.info), devs, notifier);
            }
        );

        Dev_unsafe_getter getter{devs};

        auto [begin_, end_] = getter.get_all_devs();
        std::for_each(
            begin_,
            end_,
            [&, this](auto &dev)
            {
                dev.second.get_dev().cont.run();
            }
        );
    }
} // namespace Logic
