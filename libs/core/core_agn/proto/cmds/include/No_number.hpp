#pragma once

#include <Proto_cmd.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class No_number final
        : Proto_cmd<
              Device_t
              /* Policies */,
              No_arg>
    {
    public:
        const char *get_name() const noexcept override final;
        void usr_exec(std::string &&arg) const noexcept override;

        using Proto_cmd<Device_t, No_arg>::Proto_cmd;
    };

    template <typename Device_t>
    inline const char *No_number<Device_t>::get_name() const noexcept
    {
        return Packet_defs::no_number_s.data();
    }

    template <
        typename Device_t>
    inline void No_number<Device_t>::usr_exec(std::string &&arg) const noexcept
    {
        device.helpers.notifier.error(Data_loss_except{"Requested packet was not delivered!"});
        device.restart();
    }
}