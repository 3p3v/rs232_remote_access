#pragma once

#include <Get_param.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Get_baud_rate final
        : Get_param<
              Device_t
              /* Policies */,
              Numbers_only>
    {
    public:
        const char* get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Get_param<Device_t, Numbers_only>::Get_param;
    };

    template <typename Device_t>
    inline const char *Get_baud_rate<Device_t>::get_name() const noexcept
    {
        return Get_defs::get_baud_rate_s.data();
    }

    template <
        typename Device_t>
    inline void Get_baud_rate<Device_t>::exec(std::string &&arg) const
    {
        device.timer_man.stop_timer(std::string{Get_defs::get_baud_rate_s});
        auto arg_ = Set_defs::baud_rate_trans(arg);
        device.rec.port_settings.baud_rate = arg_;
        device.serial_s.set_baud_rate(arg_);
        /* Lock commands */
        goto_exchange();
    }
}