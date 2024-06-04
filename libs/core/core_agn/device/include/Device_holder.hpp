#pragma once

#include <Device_initializer.hpp>

namespace Logic
{
    class Device_holder
    {
    public:
        using Device_initializer_ptr = std::shared_ptr<Device_initializer>;

    private:
        Device_initializer_ptr dev;

        Device_holder(Device_initializer_ptr &&dev);

    public:
        Device_prereq &get_dev() & noexcept;

        template <
            typename Dev_t,
            typename... Args_t,
            typename = std::enable_if_t<std::is_base_of_v<Device_prereq, Dev_t>>>
        static Device_holder make_device(
            Notyfier &&notyfier,
            Remote_dev &&rec,
            Args_t &&...args);
    };

    template <typename Dev_t, typename... Args_t, typename>
    inline Device_holder Device_holder::make_device(Notyfier &&notyfier, Remote_dev &&rec, Args_t &&...args)
    {
        return Device_holder{
            Device_initializer::make_shared_device<Dev_t>(
                std::move(notyfier),
                std::move(rec),
                std::forward<Args_t>(args)...)};
    }
}