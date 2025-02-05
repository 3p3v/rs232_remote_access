#pragma once

#include <Proto_cmd.hpp>
#include <Hi_defs.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Master_hi final
        : Proto_cmd<
              Device_t
              /* Policies */,
              No_arg>
    {
        void say_hi_timeout();

    public:
        const char* get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Proto_cmd<Device_t, No_arg>::Proto_cmd;
    };

    template <typename Device_t>
    inline void Master_hi<Device_t>::say_hi_timeout()
    {
        /* Inform about error */
        device.helpers.notifier.error(Timeout_except{"Saying hi timed out..."});

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
            device.helpers.notifier.error(Logic_except{"Another master operating on the same channel..."});
            device.helpers.notifier.error(Logic_except{"Reseting connection..."});
            device.reset();
        }
        else
        {
            device.helpers.notifier.debug("Master resetted the connection...");
            device.helpers.cmds.disable_all_but(Hi_defs::slave_hi_s.data());
            device.helpers.timer_man.start_timer(
                Hi_defs::slave_hi_s.data(),
                device.helpers.make_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout();
                    }));
        }
    }
}