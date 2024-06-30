#pragma once

#include <mutex>
#include <Dev_ext.hpp>
#include <Device_prereq.hpp>
#include <Dev_cont.hpp>

namespace Logic
{
    /// @brief Devices getter interface
    class Dev_getter : public Dev_ext
    {
        using Dev_ext::Dev_ext;

    public:
        using Device_prereq_and_lock = std::pair<Device_prereq &, std::lock_guard<std::mutex>>;
        using Device_iters_and_lock = std::tuple<
            Dev_cont::iterator,
            Dev_cont::iterator,
            std::lock_guard<std::mutex>>;
        using Const_Device_prereq_and_lock = std::pair<const Device_prereq &, std::lock_guard<std::mutex>>;
        using Const_Device_iters_and_lock = std::tuple<
            Dev_cont::const_iterator,
            Dev_cont::const_iterator,
            std::lock_guard<std::mutex>>;

        /// @brief Get device object and lock scope so device can't be destroyed by another thread
        /// @param num
        /// @return device and lock guard
        Device_prereq_and_lock get_dev_and_lock(Dev_num num);

        /// @brief Get all devices and lock scope so they can't be destroyed by another thread
        /// @return 
        Device_iters_and_lock get_all_devs_and_lock();

        /// @brief Get device object and lock scope so device can't be destroyed by another thread
        /// @param num
        /// @return device and lock guard
        Const_Device_prereq_and_lock get_dev_and_lock(Dev_num num) const;

        /// @brief Get all devices and lock scope so they can't be destroyed by another thread
        /// @return 
        Const_Device_iters_and_lock get_all_devs_and_lock() const;
    };
} // namespace Logic
