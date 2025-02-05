#pragma once

#include <Set_param.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Set_stop_bits final
        : Set_param<
              Device_t>
    {
    public:
        const char* get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Set_param<Device_t>::Set_param;
    };

    template <
        typename Device_t>
    const char* Set_stop_bits<Device_t>::get_name() const noexcept 
    {
        return Set_defs::set_stop_bits_s.data();
    }

    template <
        typename Device_t>
    inline void Set_stop_bits<Device_t>::exec(std::string &&arg) const
    {
        device.helpers.notifier.debug("Received request to change stop bits...");
        wait(Get_defs::get_baud_rate_s.data());
    }
}