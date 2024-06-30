#pragma once

#include <Device_initializer.hpp>

namespace Logic
{
    /// @brief Wraps Device_initializer to hide that it is a shared_ptr
    class Device_holder
    {
    public:
        using Device_initializer_ptr = std::shared_ptr<Device_initializer>;

    private:
        Device_initializer_ptr dev;

        Device_holder(Device_initializer_ptr &&dev);

    public:
        Device_prereq &get_dev() & noexcept;

        /// @brief Construct a new device
        /// @tparam Dev_t Device type
        /// @tparam ...Args_t 
        /// @param notyfier User notifier
        /// @param rec Device record
        /// @param ...args Rest of arguments for devices constructor
        /// @return New device
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