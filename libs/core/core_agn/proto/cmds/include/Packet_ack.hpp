#pragma once

#include <Data_proto.hpp>
#include <Numbers_only.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Packet_ack final
        : Data_proto<
              Device_t,
              /* Policies */
              Numbers_only>
    {
    public:
        const char *get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Data_proto<Device_t, Numbers_only>::Data_proto;
    };

    template <typename Device_t>
    inline const char *Packet_ack<Device_t>::get_name() const noexcept
    {
        return Packet_defs::packet_ack_s.data();
    }

    template <
        typename Device_t>
    inline void Packet_ack<Device_t>::exec(std::string &&arg) const
    {
        exchanger.ack(arg[0])
    }
}