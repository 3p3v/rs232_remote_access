#pragma once

#include <memory>
/* Device and extension management */
#include <Process_full.hpp>
#include <User_controller.hpp>
#include <Remote_dev.hpp>
/* Dummy interface objects */
#include <Mqtt_settings_connect.hpp>
#include <Mqtt_side_connect.hpp>
#include <Serial_side_connect.hpp>
/* Extensions */
#include <Logic.hpp>
#include <Setter.hpp>
#include <Exchanger.hpp>

namespace Logic
{
    /// @brief Representation of class contains all extensions and other objects
    ///        needed to communicate between serial and remote device in respect of protocol
    template <
        typename Timer_impl,
        typename Remote_sett_conn_impl,
        typename Remote_conn_impl,
        typename Remote_sett_impl,
        typename Remote_side_impl,
        typename Serial_conn_impl,
        typename Serial_sett_impl,
        typename Serial_side_impl>
    class Device
    {
        /**************************** Definitions ****************************/
    public:
        using Record_ptr = std::shared_ptr<Remote_dev>;

    private:
        /// @brief Manager pointer
        using Manager_ptr = std::shared_ptr<Process_full>;

        /* Extensions */
        using Logic_ext = Logic<
            Timer_impl,
            Remote_sett_impl>;
        using Setter_ext = Setter<
            Timer_impl,
            Remote_sett_impl,
            Serial_sett_impl>;
        using Exchanger_ext = Exchanger<
            Timer_impl,
            Remote_sett_impl,
            Remote_side_impl,
            Serial_side_impl>;

    private:
        /// @brief Extension management
        Manager_ptr manager{};

        /// @brief Forwarding commands from remote to interpreter
        Mqtt_settings_connect<Remote_sett_conn_impl> remote_sc;

        /// @brief Forwarding data from remote to ext
        Mqtt_side_connect<Remote_conn_impl> remote_c;

        /// @brief Forwarding data from serial to ext
        Serial_side_connect<Serial_conn_impl> serial_c;

    public:
        /// @brief Control over device connection parameters ect.
        User_controller cont{manager};

        /// @brief Current settings and status
        const Record_ptr rec;

        /// @brief Send signal to start communication with device
        void run()
        {
            manager->forward_job(Start_job{});
        }

        template <typename Record_ptr_t>
        Device(
            Remote_sett_conn_impl &&remote_sc,
            Remote_conn_impl &&remote_c,
            Remote_sett_impl &&remote_s,
            Remote_side_impl &&remote_d,
            Serial_conn_impl &&serial_c,
            Serial_sett_impl &&serial_s,
            Serial_side_impl &&serial_d,
            Record_ptr_t &&rec)
            : remote_sc{std::move(remote_sc)},
              remote_c{std::move(remote_c)},
              serial_c{std::move(serial_c)},
              rec{rec}
        {
            /* Add Logic extension */
            manager->add_ext<Logic_ext>(manager, remote_s, rec);

            /* Add Setter extension */
            manager->add_ext<Setter_ext>(manager, remote_s, serial_s, rec);

            /* Create Exchanger extension */
            auto exchanger = std::make_shared<Exchanger_ext>(
                manager,
                remote_s,
                remote_d,
                serial_d,
                std::forward<Record_ptr_t>(rec));
            /* Bind extension to implementation */
            std::apply(
                [this](auto &&...args)
                {
                    remote_c.connect(std::forward<decltype(args)>(args)...);
                },
                exchanger->get_remote_args());
            std::apply(
                [this](auto &&...args)
                {
                    serial_c.connect(std::forward<decltype(args)>(args)...);
                },
                exchanger->get_serial_args());
            /* Add to manager */
            manager->add_ext(std::move(exchanger));

            /* Connect to settings topic */
            remote_sc.connect(
                /* Callback used when receiving message */
                [manager](auto begin, auto end, auto callb)
                {
                    /* Forward message to interpreter */
                    manager->exec(begin, end);

                    /* Run callback releasing resources etc. */
                    callb();
                },
                /* Callback used when could not collect or had exception while reading */
                [](auto e)
                {
                    // TODO send error to monitor
                });
        }

        Device(Device &&) = default;
        Device &operator=(Device &&) = default;
        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;
        virtual ~Device() = default;
    };
}