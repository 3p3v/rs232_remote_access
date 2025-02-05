#pragma once

#include <Proto_cmd.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Invalid_number final
        : Proto_cmd<
              Device_t
              /* Policies */,
              No_arg>
    {
    public:
        const char *get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Proto_cmd<Device_t, No_arg>::Proto_cmd;
    };

    template <typename Device_t>
    inline const char *Invalid_number<Device_t>::get_name() const noexcept
    {
        return Packet_defs::no_number_s.data();
    }

    template <
        typename Device_t>
    inline void Invalid_number<Device_t>::exec(std::string &&arg) const
    {
        device.helpers.notifier.error(Data_loss_except{"Requested packet was not delivered!"});
        device.restart();
    }
}