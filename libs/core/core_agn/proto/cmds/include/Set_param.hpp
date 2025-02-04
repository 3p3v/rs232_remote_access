#pragma once

#include <Proto.hpp>

namespace Logic
{
    /// @brief  Base class for commands setting parameters
    /// @tparam Device_t
    template <
        typename Device_t,
        typename... Policies_t>
    class Set_param
        : Proto<
              Device_t,
              ... Policies_t>
    {
    protected:
        /// @brief  Check if all params arrived
        /// @tparam Str_t
        /// @param cmd
        template <typename Str_t>
        void wait(Str_t &&cmd) const;

        using Proto<Device_t, ... Policies_t>::Proto;
    };

    template <
        typename Device_t>
    template <typename Str_t>
    inline void Set_param<Device_t>::wait(Str_t &&cmd) const
    {
        if (device.rec.conf_port == Remote_conf_port::Non_configurable)
        {
            device.timer_man.start_timer(
                std::forward<Str_t>(cmd),
                Timer_t::make_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        device.notifier.error(Timeout_except{"Command timed out!"});
                        device.restart();
                    }));
        }
        else
        {
            device.notifier.error(Setter_except{"Another master on same channel, rebooting..."});
            device.restart();
        }
    }
}