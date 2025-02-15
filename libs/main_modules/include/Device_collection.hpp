#pragma once

#include <memory>

#include <Notification_manager.hpp>
#include <Base_device_init.hpp>

using namespace Impl;

namespace Logic
{
    class Device_collection final
    {
        using Device_tuple = std::tuple<Dev_num, std::shared_ptr<Base_proto_mediator>>;
        
        /// @brief Init of the devices
        std::unique_ptr<Base_device_init> device_init;
        /// @brief Container of devices
        std::vector<Device_tuple> devices;

    public:
        Device_collection(
            std::unique_ptr<Base_device_init> &&device_init);

        /// @brief Add devices from some container
        /// @tparam Iter_t
        /// @param begin
        /// @param end
        /// @return
        template <typename Iter_t>
        void add_devices(Notifier &&notifier, Iter_t begin, Iter_t end) &;
    };

    template <typename Iter_t>
    inline void Device_collection::add_devices(Notifier &&notifier, Iter_t begin, Iter_t end) &
    {
        /* Resize devices vector */
        devices.reserve(devices.size() + (end - begin));

        std::for_each(
            begin,
            end,
            [&, this](auto &dev)
            {
                auto d = device_init->create(
                    notifier,
                    std::move(dev.rec),
                    std::move(dev.remote_rec),
                    std::move(dev.serial_rec));

                /* Run device */
                d->start();

                devices.emplace_back(std::move(d));
            });
    }
}