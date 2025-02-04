#pragma once

#include <Exec.hpp>
#include <Device.hpp>

namespace Logic
{
    template <
        typename Device_t,
        typename... Policies_t,
        typename = typename std::enable_if_t<
            std::is_base_of_v<
                Device_prereq, // TODO check if base of Device, not Device_prereq
                typename std::decay_t<Device_t>>>>
    class Proto : public Exec<Policies_t...>
    {
    protected:
        Device_t &dev;

        std::shared_ptr<Device_prereq> shared_from_this();
        std::weak_ptr<Device_prereq> weak_from_this();

        auto Proto<Policies_t...>::def_ec_callb();

    public:
        virtual const char* get_name() const noexcept = 0;

        Proto(Device_t &dev);
    };

    template <
        typename Device_t,
        typename... Policies_t,
        typename>
    std::shared_ptr<Device_prereq> Proto<Policies_t...>::shared_from_this()
    {
        return device.get_shared();
    }

    template <
        typename Device_t,
        typename... Policies_t,
        typename>
    std::weak_ptr<Device_prereq> Proto<Policies_t...>::weak_from_this()
    {
        return device.get_weak();
    }

    template <
        typename Device_t,
        typename... Policies_t,
        typename>
    auto Proto<Policies_t...>::def_ec_callb()
    {
        return [](const std::exception &e) {};
    }

    template <
        typename Device_t,
        typename... Policies_t,
        typename>
    Proto<Policies_t...>::Proto(Device_t &dev)
        : dev{dev}
    {
    }
}
