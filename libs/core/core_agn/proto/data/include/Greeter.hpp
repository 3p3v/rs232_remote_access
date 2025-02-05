#pragma once

#include <Proto_module.hpp>

namespace Logic
{
    class Base_greeter final : public Base_proto_module
    {
    public:
        virtual void greet() = 0;
    };

    template <typename Device_t>
    class Greeter : public Base_greeter, public Proto_commons<Device_t>
    {
        void timeout();

    public:
        void restart() override;
        void greet() override;

        using Proto_commons<Device_t>::Proto_commons;
    };

    template <typename Device_t>
    inline void Greeter<Device_t>::timeout()
    {
        /* Inform about error */
        device.helpers.notifier.error(Timeout_except{"Saying hi timed out..."});
        greet();
    }

    template <typename Device_t>
    inline void Greeter<Device_t>::restart()
    {
        /* Inform about error */
        device.helpers.notifier.error(Timeout_except{"Restarting..."});
        greet();
    }

    template <typename Device_t>
    inline void Greeter<Device_t>::greet()
    {
        device.helpers.notifier.debug("Welcoming the device.helpers...");

        if (device.helpers.rec.conf_port == Remote_conf_port::Configurable)
        {
            device.helpers.remote_s.write_s(
                Hi_defs::master_hi_s.data(),
                [serial_ctrl = shared_from_this(), this]()
                {
                    device.helpers.timer_man.start_timer(
                        Hi_defs::slave_hi_s.data(),
                        Timer_t::make_timer(
                            [serial_ctrl = shared_from_this(), this]()
                            {
                                /* Try to say hi to device.helpers again */
                                timeout();
                            }));
                },
                def_ec_callb());
        }
        else
        {
            device.helpers.remote_s.write_i(
                Hi_defs::master_keep_alive_s.data(),
                [serial_ctrl = shared_from_this(), this]()
                {
                    device.helpers.timer_man.start_timer(
                        Hi_defs::slave_keep_alive_s.data(),
                        Timer_t::make_timer(
                            [serial_ctrl = shared_from_this(), this]()
                            {
                                /* Try to say hi to device.helpers again */
                                timeout();
                            }));
                },
                def_ec_callb());
        }
    }
}