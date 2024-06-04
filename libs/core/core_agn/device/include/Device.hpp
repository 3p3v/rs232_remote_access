#pragma once

#include <memory>
/*  */
#include <Device_prereq.hpp>
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
        typename Timer_impl_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    class Device : public Device_prereq
    {
        /**************************** Definitions ****************************/
    private:
        /* Extensions */
        using Logic_ext = Logic<
            Timer_impl_t,
            Remote_impl_t>;
        using Setter_ext = Setter<
            Timer_impl_t,
            Remote_impl_t,
            Serial_impl_t>;
        using Exchanger_ext = Exchanger<
            Timer_impl_t,
            Remote_impl_t,
            Remote_impl_t,
            Serial_impl_t>;

    private:
        /// @brief Forwarding commands from remote to interpreter
        Mqtt_settings_connect<Remote_impl_t> remote_sc;

        Remote_impl_t remote;
        Serial_impl_t serial;

    public:
        template <
            typename Device_ptr_t,
            typename Remote_impl_ptr_t,
            typename Serial_impl_ptr_t>
        Device(
            Notyfier &&notyfier,
            Device_ptr_t &&dev,
            Remote_dev &&rec,
            Remote_impl_ptr_t &&remote,
            Serial_impl_ptr_t &&serial);

        Device(Device &&) = default;
        Device &operator=(Device &&) = default;
        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;
        virtual ~Device() = default;
    };

    template <
        typename Timer_impl_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    template <
        typename Device_ptr_t,
        typename Remote_impl_ptr_t,
        typename Serial_impl_ptr_t>
    inline Device<
        Timer_impl_t,
        Remote_impl_t,
        Serial_impl_t>::Device(
            Notyfier &&notyfier_,
            Device_ptr_t &&dev_,
            Remote_dev &&rec__,
            Remote_impl_ptr_t &&remote_,
            Serial_impl_ptr_t &&serial_)
        : Device_prereq{std::move(notyfier_), std::forward<Device_ptr_t>(dev_), std::move(rec__)},
          remote_sc{remote_},
          remote{std::forward<Remote_impl_ptr_t>(remote_)},
          serial{std::forward<Serial_impl_ptr_t>(serial_)}
    {
        /* Add Logic extension */
        manager.add_ext<Logic_ext>(
            Forwarder{manager.ext_forwarder}, 
            Notyfier{notyfier}, 
            dev, 
            remote, 
            rec_);

        /* Add Setter extension */
        manager.add_ext<Setter_ext>(
            Forwarder{manager.ext_forwarder}, 
            Notyfier{notyfier}, 
            dev, 
            remote, 
            serial, 
            rec_);

        /* Create Exchanger extension */
        auto exchanger = Exchanger_ext::make(
            Forwarder{manager.ext_forwarder},
            Notyfier{notyfier},
            dev,
            remote,
            remote,
            remote,
            serial,
            serial,
            rec_);

        /* Add to manager */
        manager.add_ext(std::move(exchanger));

        /* Connect to settings topic */
        remote_sc.connect(
            /* Callback used when receiving message */
            [this, ptr = dev](auto begin, auto end, auto callb)
            {
                if (auto p = ptr.lock())
                {
                    /* Forward message to interpreter */
                    manager.cmd_exec.exec(begin, end);

                    /* Run callback releasing resources etc. */
                    callb();
                }
            },
            /* Callback used when could not collect or had exception while reading */
            [](const auto &e)
            {
                // TODO send error to monitor
            });
    }
}