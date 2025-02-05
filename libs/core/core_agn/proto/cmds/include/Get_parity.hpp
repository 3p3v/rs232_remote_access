#pragma once

#include <Get_param.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Get_parity final
        : Get_param<
              Device_t>
    {
    public:
        const char* get_name() const noexcept override final;
        void exec(std::string &&arg) const override;

        using Get_param<Device_t>::Get_param;
    };

    template <
        typename Device_t>
    inline void Get_parity<Device_t>::exec(std::string &&arg) const
    {
        device.helpers.timer_man.stop_timer(std::string{Get_defs::get_parity_s});
        auto arg_ = Set_defs::parity_trans(arg);
        device.helpers.rec.port_settings.parity = arg_;
        device.helpers.serial_s.set_parity(arg_);
        /* Lock commands */
        goto_exchange();
    }

    template <typename Device_t>
    inline const char *Get_parity<Device_t>::get_name() const noexcept
    {
        return Get_defs::get_parity_s.data();
    }
}