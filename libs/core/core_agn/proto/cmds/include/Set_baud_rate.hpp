#pragma once

#include <Set_param.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Set_baud_rate final
        : Set_param<
              Device_t
              /* Policies */,
              Numbers_only>
    {
    public:
        const char* get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Set_param<Device_t, Numbers_only>::Set_param;
    };

    template <typename Device_t>
    inline const char *Set_baud_rate<Device_t>::get_name() const noexcept
    {
        return Set_defs::set_baud_rate_s.data();
    }

    template <
        typename Device_t>
    inline void Set_baud_rate<Device_t>::exec(std::string &&arg) const
    {
        device.helpers.notifier.debug("Received request to change baud rate...");
        wait(Get_defs::get_baud_rate_s.data());
    }
}