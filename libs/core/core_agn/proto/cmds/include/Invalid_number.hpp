#pragma once

#include <Data_proto.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Invalid_number final
        : Data_proto<
              Device_t
              /* Policies */,
              No_arg>
    {
    public:
        const char *get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Data_proto<Device_t, No_arg>::Data_proto;
    };

    template <typename Device_t>
    inline const char *Invalid_number<Device_t>::get_name() const noexcept
    {
        return Packet_defs::invalid_number_s.data();
    }

    template <
        typename Device_t>
    inline void Invalid_number<Device_t>::exec(std::string &&arg) const
    {
        /* Delete packet from memory */
        auto &p_num = arg[0];
        device.helpers.notifier.debug("Resending from packet: " + std::string{p_num} + "...");
        exchanger.resend(p_num);
    }
}