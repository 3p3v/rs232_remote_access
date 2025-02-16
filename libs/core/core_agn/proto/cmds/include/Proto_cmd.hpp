#pragma once

#include <Exec.hpp>
#include <Exec_en.hpp>
#include <Exec_policy.hpp>
#include <Proto_commons.hpp>

using namespace Cmd_ctrl;

namespace Logic
{
    class Base_proto_cmd : public Exec<Exec_en>
    {
        using Exec_en::Exec_en;
    };

    template <
        typename Device_t,
        typename... Policies_t>
    class Proto_cmd
        : public Exec_policy<Base_proto_cmd, Policies_t...>,
          public Proto_commons<Device_t>
    {
    protected:
        Device_t device;

    public:
        virtual const char *get_name() const noexcept = 0;

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
