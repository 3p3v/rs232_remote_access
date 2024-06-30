#pragma once

#include <Dev_getter.hpp>
#include <Info_getter.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Interface for retrieving devices and their informatipon
    class Impl_getter final : public Info_getter, public Dev_getter
    {
    public:
        using Dev_num = Impl_ext::Dev_num;
        using Info_Dev_prereq_and_lock = std::tuple<Dev_info &, Device_prereq &, std::lock_guard<std::mutex>>;
        using Const_Info_Dev_prereq_and_lock = std::tuple<const Dev_info &, const Device_prereq &, std::lock_guard<std::mutex>>;

        /// @brief Get device and its info, locks access to all devices from other threads
        /// @param num Devices number
        /// @return Reference to information, reference to device and a lock guard
        Info_Dev_prereq_and_lock get_and_lock(Dev_num num);

        /// @brief Get constant device and its info, locks access to all devices from other threads
        /// @param num Devices number
        /// @return Reference to const information, reference to const device and a lock guard
        Const_Info_Dev_prereq_and_lock get_and_lock(Dev_num num) const;

        Impl_getter(Impl_cont &devs);
    };
} // namespace Logic
