#pragma once

#include <Proto_cmd.hpp>
#include <Hi_defs.hpp>

namespace Logic
{
    template<
        typename Device_t>
    class Keep_alive final
        : Proto_cmd<
            Device_t
            /* Policies */,
            No_arg>
    {
    public:
        const char* get_name() const noexcept override final;
        void usr_exec(std::string &&arg) const noexcept override;
    
        using Proto_cmd<Device_t, No_arg>::Proto_cmd;
    };

    template <typename Device_t>
    inline const char *Keep_alive<Device_t>::get_name() const noexcept
    {
        return Hi_defs::keep_alive_s.data();
    }

    template <
        typename Device_t>
    inline void Keep_alive<Device_t>::usr_exec(std::string &&arg) const noexcept
    {
        device.helpers.notifier.debug("Received keep alive.");
    }
}