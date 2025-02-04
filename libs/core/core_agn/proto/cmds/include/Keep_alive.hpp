#pragma once

#include <Proto.hpp>

namespace Logic
{
    template<
        typename Device_t>
    class Keep_alive final
        : Proto<
            Device_t
            /* Policies */,
            No_arg>
    {
    public:
        const char* get_name() const noexcept override final;
        void exec(std::string &&arg) const override;
    
        using Proto<Device_t, No_arg>::Proto;
    };

    template <typename Device_t>
    inline const char *Keep_alive<Device_t>::get_name() const noexcept
    {
        return Hi_defs::keep_alive_s.data();
    }

    template <
        typename Device_t>
    inline void Keep_alive<Device_t>::exec(std::string &&arg) const
    {
        notifier.debug("Received keep alive.");
    }
}