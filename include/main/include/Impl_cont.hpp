#pragma once

#include <map>
#include <mutex>
#include <Notyfier_defs.hpp>
#include <Dev_info.hpp>

namespace Impl
{
    class Impl_cont
    {
    public:
        using Dev_num = ::Logic::Notyfier_defs::Dev_num;
        using Info_num = Dev_num;
        using Info_cont_t = std::unordered_map<Dev_num, Dev_info>;
        using iterator = Info_cont_t::iterator;

        /// @brief Additional info about devices
        Info_cont_t infos;

        /// @brief 
        Info_num num{0};

        /// @brief Protect from access to devices  from multiple threads
        std::mutex info_mutex{};
    };
} // namespace Impl
