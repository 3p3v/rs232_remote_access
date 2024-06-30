#pragma once

#include <mutex>
#include <Dev_ext.hpp>
#include <Device_prereq.hpp>
#include <Dev_cont.hpp>

namespace Logic
{
    /// @brief Devices getter interface, 
    ///        not safe for multithreading as it does not use mutexes
    class Dev_unsafe_getter : public Dev_ext
    {
        using Dev_ext::Dev_ext;

    public:
        using Device_iters = std::tuple<
            Dev_cont::iterator,
            Dev_cont::iterator>;
        using Const_device_iters = std::tuple<
            Dev_cont::const_iterator,
            Dev_cont::const_iterator>;

        /// @brief Get device object
        /// @param num
        /// @return device and lock guard
        Device_prereq &get_dev(Dev_num num);

        /// @brief Get all devices 
        /// @return 
        Device_iters get_all_devs();

        /// @brief Get device object 
        /// @param num
        /// @return device and lock guard
        const Device_prereq &get_dev(Dev_num num) const;

        /// @brief Get all devices 
        /// @return 
        Const_device_iters get_all_devs() const;
    };
} // namespace Logic
