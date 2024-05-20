#pragma once

#include <mutex>
#include <Dev_ext.hpp>
#include <Device_prereq.hpp>
#include <Dev_cont.hpp>

namespace Logic
{
    class Dev_getter : public Dev_ext
    {
        using Dev_ext::Dev_ext;

    public:
        using Device_prereq_and_lock = std::pair<Device_prereq &, std::lock_guard<std::mutex>>;
        using Device_iters_and_lock = std::tuple<
            Dev_cont::iterator,
            Dev_cont::iterator,
            std::lock_guard<std::mutex>>;

        /// @brief Get device object and lock scope so bject can't be destroyed by another thread
        /// @param num
        /// @return device and lock guard
        Device_prereq_and_lock get_and_lock(Dev_num num) const;

        Device_iters_and_lock get_all_and_lock() const;
    };
} // namespace Logic
