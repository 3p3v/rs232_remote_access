#pragma once

#include <Proto_cmd.hpp>
#include <Get_defs.hpp>

namespace Logic
{
    /// @brief  Base class for commands getting parameters
    /// @tparam Device_t
    template <
        typename Device_t,
        typename... Policies_t>
    class Get_param
        : Proto_cmd<
              Device_t,
              ... Policies_t>
    {
    protected:
        /// @brief  Check if all params arrived
        void goto_exchange() const;

        using Proto_cmd<Device_t, Numbers_only>::Proto_cmd;
    };

    template <
        typename Device_t>
    inline void Get_param<Device_t>::goto_exchange() const
    {
        device.helpers.rec.params_established++;
        if (device.helpers.rec.params_established == device.helpers.rec.all_established)
        {
            device.cmds.disable_all_but(
                {Packet_defs::packet_ack_s.data(),
                 Packet_defs::invalid_number_s.data(),
                 Packet_defs::no_number_s.data()});
        }
        else if (device.helpers.rec.params_established > device.helpers.rec.all_established)
        {
            throw Setter_except{"Internal error. Established more parameters than max."};
        }
        else
        {
            device.cmds.disable_cmd(get_name());
        }
    }
}