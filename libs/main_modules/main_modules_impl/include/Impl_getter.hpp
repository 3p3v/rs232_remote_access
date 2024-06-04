#pragma once

#include <Dev_getter.hpp>
#include <Impl_cont.hpp>

using namespace Logic;

namespace Impl
{
    class Impl_getter : public Dev_getter
    {
        /// @brief 
        Impl_cont &infos;

    public:
        using Info_prereq_and_lock = std::pair<Dev_info &, std::lock_guard<std::mutex>>;
        using Info_iters_and_lock = std::tuple<
            Impl_cont::iterator,
            Impl_cont::iterator,
            std::lock_guard<std::mutex>>;

        /// @brief Get device object and lock scope so object can't be destroyed by another thread
        /// @param num
        /// @return device and lock guard
        Info_prereq_and_lock get_infos_and_lock(Dev_num num) const;

        Info_iters_and_lock get_all_infos_and_lock() const;

        Impl_getter(Dev_cont &devs, Impl_cont &infos);
    };
} // namespace Logic
