#pragma once

#include <Set_param.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Set_parity final
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
    const char* Set_parity<Device_t>::get_name() const noexcept 
    {
        return Set_defs::set_parity_s.data();
    }

    template <
        typename Device_t>
    inline void Set_parity<Device_t>::exec(std::string &&arg) const
    {
        device.notifier.debug("Received request to change parity...");
        wait(Get_defs::get_parity_s.data());
    }
}