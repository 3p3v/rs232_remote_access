#pragma once

#include <type_traits>
#include <Device.hpp>

namespace Logic
{
    template <
        typename Device_t>
    // typename = typename std::enable_if_t<
    //     std::is_base_of_v<
    //         Device_t, // TODO check if base of Device, not Device_t
    //         typename std::decay_t<Device_t>>>>
    class Proto_commons
    {
    protected:
        Device_t &device;

        std::shared_ptr<Device_t> shared_from_this();
        std::weak_ptr<Device_t> weak_from_this();

        auto def_ec_callb();

    public:
        Proto_commons(Device_t &device)
    };

    template <typename Device_t>
    std::shared_ptr<Device_t> Proto_commons<Device_t>::shared_from_this()
    {
        return device.get_shared();
    }

    template <typename Device_t>
    std::weak_ptr<Device_t> Proto_commons<Device_t>::weak_from_this()
    {
        return device.get_weak();
    }

    template <typename Device_t>
    auto Proto_commons<Device_t>::def_ec_callb()
    {
        return [](const std::exception &e) {};
    }

    template <typename Device_t>
    Proto_commons<Device_t>::Proto_commons(Device_t &device)
        : device{device}
    {
    }
}