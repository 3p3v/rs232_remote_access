#pragma once

#include <Exec.hpp>
#include <Proto_commons.hpp>

namespace Logic
{
    template <
        typename Device_t,
        typename... Policies_t>
    class Proto_cmd : public Exec<Policies_t...>, public Proto_commons<Device_t>
    {
    protected:
        Device_t device;

    public:
        virtual const char* get_name() const noexcept = 0;

        Proto_cmd(Device_t &device);
    };

    template <
        typename Device_t,
        typename... Policies_t>
    Proto_cmd<Device_t, Policies_t...>::Proto_cmd(Device_t &device)
        : device{device}
    {
    }
}
