#pragma once

#include <Get_param.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Get_char_size final
        : Get_param<
              Device_t
              /* Policies */,
              Numbers_only>
    {
    public:
        const char *get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Get_param<Device_t, Numbers_only>::Get_param;
    };

    template <
        typename Device_t>
    inline void Get_char_size<Device_t>::exec(std::string &&arg) const
    {
        device.helpers.timer_man.stop_timer(std::string{Get_defs::get_char_size_s});
        auto arg_ = Set_defs::char_size_trans(arg);
        device.helpers.rec.port_settings.char_size = arg_;
        device.helpers.serial_s.set_char_size(arg_);
        /* Lock commands */
        goto_exchange();
    }

    template <typename Device_t>
    inline const char *Get_char_size<Device_t>::get_name() const noexcept
    {
        return Get_defs::get_char_size_s.data();
    }
}