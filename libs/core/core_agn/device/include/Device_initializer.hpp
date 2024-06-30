#pragma once

#include <memory>
#include <stdexcept>
#include <Device_base.hpp>
#include <Notyfier.hpp>
#include <Remote_dev.hpp>

namespace Logic
{
    class Device_prereq;

    /// @brief Constructs a device in form of a shared_ptr
    class Device_initializer
        : public Device_base,
          public std::enable_shared_from_this<Device_initializer>
    {
        std::unique_ptr<Device_prereq> dev{nullptr};

        template <
            typename Dev_t,
            typename... Args_t,
            typename = std::enable_if_t<std::is_base_of_v<Device_prereq, Dev_t>>>
        void create_dev(
            Notyfier &&notyfier,
            Remote_dev &&rec,
            Args_t &&...args) & noexcept;

        Device_initializer() = default;

    public:
        /// @brief Get reference to the device
        /// @return 
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
        static auto make_shared_device(
            Notyfier &&notyfier,
            Remote_dev &&rec,
            Args_t &&...args);
    };

    template <typename Dev_t, typename... Args_t, typename>
    inline void Device_initializer::create_dev(Notyfier &&notyfier, Remote_dev &&rec, Args_t &&...args) & noexcept
    {
        dev = std::make_unique<Dev_t>(
            std::move(notyfier),
            weak_from_this(),
            std::move(rec),
            std::forward<Args_t>(args)...);
    }

    template <typename Dev_t, typename... Args_t, typename>
    inline auto Device_initializer::make_shared_device(Notyfier &&notyfier, Remote_dev &&rec, Args_t &&...args)
    {
        auto dev = std::make_shared<Device_initializer>(Device_initializer{});

        dev->create_dev<Dev_t, Args_t...>(
            std::move(notyfier),
            std::move(rec),
            std::forward<Args_t>(args)...);

        return dev;
    }
} // namespace Logic
