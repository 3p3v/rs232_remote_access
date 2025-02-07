#pragma once

#include <Proto_cmd.hpp>
#include <Hi_defs.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Slave_hi final
        : Proto_cmd<
              Device_t,
              /* Policies */
              No_arg>
    {
        void set_baud_rate(Port_settings::Baud_rate arg);
        void set_parity(Port_settings::Parity arg);
        void set_char_size(Port_settings::Char_size arg);
        void set_stop_bits(Port_settings::Stop_bits arg);

        void get_baud_rate();
        void get_parity();
        void get_char_size();
        void get_stop_bits();

        void get_parameters();

    public:
        const char *get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Proto_cmd<Device_t, No_arg>::Proto_cmd;
    };

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::set_baud_rate(const Port_settings::Baud_rate arg)
    {
        device.helpers.remote_s.write_s(
            std::string{Set_defs::set_baud_rate_s},
            Set_defs::baud_rate_trans(arg),
            device.make_shared(
                [this]()
                {
                    get_baud_rate();
                }),
            def_ec_callb());
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::set_parity(const Port_settings::Parity arg)
    {
        device.helpers.remote_s.write_s(
            std::string{Set_defs::set_parity_s},
            Set_defs::parity_trans(arg),
            device.make_shared(
                [this]()
                {
                    get_parity();
                }),
            def_ec_callb());
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::set_char_size(const Port_settings::Char_size arg)
    {
        device.helpers.remote_s.write_s(
            std::string{Set_defs::set_char_size_s},
            Set_defs::char_size_trans(arg),
            device.make_shared(
                [this]()
                {
                    get_char_size();
                }),
            def_ec_callb());
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::set_stop_bits(const Port_settings::Stop_bits arg)
    {
        device.helpers.remote_s.write_s(
            std::string{Set_defs::set_stop_bits_s},
            Set_defs::stop_bits_trans(arg),
            device.make_shared(
                [this]()
                {
                    get_stop_bits();
                }),
            def_ec_callb());
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::get_parameters()
    {
        get_baud_rate();
        get_parity();
        get_char_size();
        get_stop_bits();
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::get_baud_rate()
    {
        device.helpers.timer_man.start_timer(
            std::string{Get_defs::get_baud_rate_s},
            Timer_t::make_timer(
                device.make_weak(
                    [this]()
                    {
                        /* Try to say hi to device.helpers again */
                        device.helpers.notifier.error(Timeout_except{"Command timed out!"});

                        device.reset();
                    })));
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::get_parity()
    {
        device.helpers.timer_man.start_timer(
            std::string{Get_defs::get_parity_s},
            Timer_t::make_timer(
                device.make_weak(
                    [this]()
                    {
                        /* Try to say hi to device.helpers again */
                        device.helpers.notifier.error(Timeout_except{"Command timed out!"});

                        device.reset();
                    })));
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::get_char_size()
    {
        device.helpers.timer_man.start_timer(
            std::string{Get_defs::get_char_size_s},
            Timer_t::make_timer(
                device.make_weak(
                    [this]()
                    {
                        /* Try to say hi to device.helpers again */
                        device.helpers.notifier.error(Timeout_except{"Command timed out!"});

                        device.reset();
                    })));
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::get_stop_bits()
    {
        device.helpers.timer_man.start_timer(
            std::string{Get_defs::get_stop_bits_s},
            Timer_t::make_timer(
                device.make_weak(
                    [this]()
                    {
                        /* Try to say hi to device.helpers again */
                        device.helpers.notifier.error(Timeout_except{"Command timed out!"});

                        device.reset();
                    })));
    }

    template <typename Device_t>
    inline const char *Slave_hi<Device_t>::get_name() const noexcept
    {
        return Hi_defs::slave_hi_s.data();
    }

    template <
        typename Device_t>
    inline void Slave_hi<Device_t>::exec(std::string &&arg) const
    {
        device.helpers.timer_man.stop_timer(Hi_defs::slave_hi_s.data());

        /* Change status */
        device.helpers.rec.status = Remote_status::Establishing_parameters;

        /* Enable only config commands */
        device.cmds.disable_all();

        /* Start establishing parameters */
        if (device.helpers.rec.conf_port == Remote_conf_port::Configurable)
        {
            device.helpers.notifier.debug("Establishing connection parameters...");
            if (device.helpers.rec.settings_known)
            {
                /* Send parameters */
                set_baud_rate(device.helpers.rec.port_settings.baud_rate);
                set_parity(device.helpers.rec.port_settings.parity);
                set_char_size(device.helpers.rec.port_settings.char_size);
                set_stop_bits(device.helpers.rec.port_settings.stop_bits);
            }
            else
            {
                get_parameters();
            }
        }
        else
        {
            device.helpers.notifier.debug("Waiting for master to establish connection parameters...");
        }
    }
}