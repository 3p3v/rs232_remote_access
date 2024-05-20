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

        /// @brief Forwarding data from remote to ext
        Mqtt_side_connect<Remote_impl_t> remote_c;

        /// @brief Forwarding data from serial to ext
        Serial_side_connect<Serial_impl_t> serial_c;

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
            Notyfier &&notyfier,
            Device_ptr_t &&dev,
            Remote_dev &&rec,
            Remote_impl_ptr_t &&remote,
            Serial_impl_ptr_t &&serial)
        : Device_prereq{std::move(notyfier), std::forward<Device_ptr_t>(dev), std::move(rec)},
          remote_sc{remote},
          remote_c{remote},
          serial_c{serial}
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
        auto exchanger = Exchanger_ext{
            Forwarder{manager.ext_forwarder},
            Notyfier{notyfier},
            dev,
            remote,
            remote,
            serial,
            rec_};
        /* Bind extension to implementation */
        std::apply(
            [this](auto &&...args)
            {
                remote_c.connect(std::forward<decltype(args)>(args)...);
            },
            exchanger.get_remote_args());
        std::apply(
            [this](auto &&...args)
            {
                serial_c.connect(std::forward<decltype(args)>(args)...);
            },
            exchanger.get_serial_args());
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
            [](auto e)
            {
                // TODO send error to monitor
            });
    }
}