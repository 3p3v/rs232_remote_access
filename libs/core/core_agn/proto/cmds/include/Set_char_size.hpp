#pragma once

#include <Proto.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Set_char_size final
        : Set_param<
              Device_t
              /* Policies */,
              Numbers_only>
    {
    public:
        const char* get_name() const noexcept override final;
        void usr_exec(std::string &&arg) const noexcept override;

        using Set_param<Device_t, Numbers_only>::Set_param;
    };

    template <typename Device_t>
    inline const char *Set_char_size<Device_t>::get_name() const noexcept
    {
        return Set_defs::set_char_size_s.data();
    }

    template <
        typename Device_t>
    inline void Set_char_size<Device_t>::usr_exec(std::string &&arg) const noexcept
    {
        device.helpers.notifier.debug("Received request to change char size...");
        wait(Get_defs::get_char_size_s.data());
    }
}