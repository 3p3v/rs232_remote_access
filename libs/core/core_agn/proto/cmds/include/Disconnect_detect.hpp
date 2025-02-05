#pragma once

#include <Proto_cmd.hpp>
#include <Disconnect_defs.hpp>

namespace Logic
{
    template<
        typename Device_t>
    class Disconnect_detect final
        : Proto_cmd<
            Device_t
            /* Policies */,
            No_arg>
    {
    public:
        const char* get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Proto_cmd<Device_t, No_arg>::Proto_cmd;
    };

    template <typename Device_t>
    inline const char *Disconnect_detect<Device_t>::get_name() const noexcept
    {
        return Disconnect_defs::disconnect_detect_s.data();
    }

    template <
        typename Device_t>
    inline void Disconnect_detect<Device_t>::exec(std::string &&arg) const
    {
        device.helpers.notifier.error(Disconnect_except{"Server sent device disconnected!"});
        device.reset();
    }
}