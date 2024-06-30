#pragma once

#include <Dev_unsafe_getter.hpp>
#include <Impl_adder_ref_base.hpp>
#include <Impl_remover.hpp>
#include <Debug_monitor.hpp>
#include <Exception_handler.hpp>
#include <Info_pack_base.hpp>
#include <memory>

using namespace Logic;

namespace Impl
{
    /// @brief Contains all nessesary components to run the application (device containers, info containers, debug, error handling, ect.)
    class Collective_general final
    {
        /// @brief Underlying low-level MQTT driver
        std::unique_ptr<Impl_adder_ref_base> adder; 

        /// @brief Devices and their additional information
        Impl_cont devs{};
        /// @brief Information about debug and errors
        Worker_storage notifyer{};

        /// @brief User debug information
        Debug_monitor debug_monitor;
        /// @brief Handling exceptioons
        Exception_handler exception_handler;

    public:
        /// @brief 
        /// @param adder Object that is able to add devices by knowing its low-level implementation
        /// @param close_on_timeout 
        /// @param close_on_data_loss 
        /// @param close_on_protocol_error 
        /// @param debug 
        Collective_general(
            std::unique_ptr<Impl_adder_ref_base> &&adder,
            bool close_on_timeout,
            bool close_on_data_loss,
            bool close_on_protocol_error,
            bool debug);

        /// @brief 
        /// @tparam Impl_adder_ref_t 
        /// @tparam  
        /// @param adder Object that is able to add devices by knowing its low-level implementation
        /// @param close_on_timeout 
        /// @param close_on_data_loss 
        /// @param close_on_protocol_error 
        /// @param debug 
        template <
            typename Impl_adder_ref_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Impl_adder_ref_base,
                    std::decay_t<Impl_adder_ref_t>>>>
        Collective_general(
            Impl_adder_ref_t &&adder,
            bool close_on_timeout,
            bool close_on_data_loss,
            bool close_on_protocol_error,
            bool debug);

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
        void add_devces(Iter_t begin, Iter_t end) &;
    };

    template <
        typename Impl_adder_ref_t, 
        typename>
    inline Collective_general::Collective_general(
        Impl_adder_ref_t &&adder, 
        bool close_on_timeout, 
        bool close_on_data_loss, 
        bool close_on_protocol_error, 
        bool debug)
        : Collective_general{
            std::make_unique<Impl_adder_ref_t>(std::forward<Impl_adder_ref_t>(adder)),
            close_on_timeout, 
            close_on_data_loss, 
            close_on_protocol_error, 
            debug
        }
    {
    }

    template <typename Iter_t>
    inline void Collective_general::add_devces(Iter_t begin, Iter_t end) &
    {   
        std::for_each(
            begin,
            end,
            [&, this](auto &dev)
            {
                adder.add(std::move(dev.sett), dev.write_access, std::move(dev.info), devs, notyfier);
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
