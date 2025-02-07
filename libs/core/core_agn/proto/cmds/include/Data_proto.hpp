#pragma once

#include <Proto_cmd.hpp>
#include <Exchanger.hpp>
#include <Packet_defs.hpp>

namespace Logic
{
    /// @brief  Base class for commands getting parameters
    /// @tparam Device_t
    template <
        typename Device_t,
        typename... Policies_t>
    class Data_proto
        : Proto_cmd<
              Device_t,
              ... Policies_t>
    {
    protected:
        Exchanger<Device_t> &exchanger;

    public:
        Data_proto(Device_t &device, Exchanger<Device_t> &exchanger);
    };

    template <typename Device_t, typename... Policies_t>
    inline Data_proto<Device_t, Policies_t...>::Data_proto(Device_t &device, Exchanger<Device_t> &exchanger)
        : Proto_cmd<Device_t, ... Policies_t>{
              device},
          exchanger{exchanger}
    {
    }
}