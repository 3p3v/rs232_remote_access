#pragma once

#include <Impl_ext.hpp>
#include <Impl_cont.hpp>

namespace Impl
{
    /// @brief Interface for retrieving devices information
    class Info_getter : public Impl_ext
    {
    public:
        using Info_prereq_and_lock = std::pair<Dev_info &, std::lock_guard<std::mutex>>;
        using Info_iters_and_lock = std::tuple<
            Impl_cont::iterator,
            Impl_cont::iterator,
            std::lock_guard<std::mutex>>;
        using Const_Info_prereq_and_lock = std::pair<const Dev_info &, std::lock_guard<std::mutex>>;
        using Const_Info_iters_and_lock = std::tuple<
            Impl_cont::const_iterator,
            Impl_cont::const_iterator,
            std::lock_guard<std::mutex>>;

        /// @brief Get devices information, locks access to all devices from other threads
        /// @param num Devices number
        /// @return Reference to information and lock guard
        Info_prereq_and_lock get_info_and_lock(Dev_num num);

        /// @brief Get all devices information, locks access to all devices from other threads
        /// @return Iteratior to information references and lock guard
        Info_iters_and_lock get_all_infos_and_lock();

        /// @brief Get const devices information, locks access to all devices from other threads
        /// @param num Devices number
        /// @return Reference to const information and lock guard
        Const_Info_prereq_and_lock get_info_and_lock(Dev_num num) const;

        /// @brief Get all devices information, locks access to all devices from other threads
        /// @return Iteratior to const information references and lock guard
        Const_Info_iters_and_lock get_all_infos_and_lock() const;

        Info_getter(Impl_cont &devs);
    };
} // namespace Impl
