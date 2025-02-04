#pragma once

#include <Proto.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Master_hi final
        : Proto<
              Device_t
              /* Policies */,
              No_arg>
    {
    public:
        const char* get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Proto<Device_t, No_arg>::Proto;
    };

    template <typename Device_t>
    inline void Master_hi<Device_t>::say_hi_timeout()
    {
        /* Inform about error */
        notifier.error(Timeout_except{"Saying hi timed out..."});

        /* Reset */
        device.reset();
    }

    template <typename Device_t>
    inline const char *Master_hi<Device_t>::get_name() const noexcept
    {
        return Hi_defs::slave_hi_s.data();
    }

    template <
        typename Device_t>
    inline void Master_hi<Device_t>::exec(std::string &&arg) const
    {
        if (remote_rec.conf_port == Remote_conf_port::Configurable)
        {
            notifier.error(Logic_except{"Another master operating on the same channel..."});
            notifier.error(Logic_except{"Reseting connection..."});
            device.reset();
        }
        else
        {
            notifier.debug("Master resetted the connection...");
            device.cmds.disable_all_but(Hi_defs::slave_hi_s.data());
            device.timer_man.start_timer(
                Hi_defs::slave_hi_s.data(),
                device.make_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout();
                    }));
        }
    }
}