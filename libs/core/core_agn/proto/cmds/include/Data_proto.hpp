#pragma once

#include <Proto.hpp>
#include <Exchanger.hpp>

namespace Logic
{
    /// @brief  Base class for commands getting parameters
    /// @tparam Device_t
    template <
        typename Device_t,
        typename... Policies_t>
    class Data_proto
        : Proto<
              Device_t,
              ... Policies_t>
    {
    protected:
        Exchanger<Device_t> &exchanger;

    public:
        Data_proto(Exchanger<Device_t> &exchanger);
    };

    template <typename Device_t, typename... Policies_t>
    inline Data_proto<Device_t, Policies_t...>::Data_proto(Exchanger<Device_t> &exchanger)
        : exchanger{exchanger}
    {
    }
}