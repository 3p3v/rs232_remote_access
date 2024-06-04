#pragma once

#include <unordered_map>
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
        mutable std::mutex info_mutex{};

        Impl_cont() = default;
        Impl_cont(Impl_cont &&);
        Impl_cont& operator=(Impl_cont &&);
        Impl_cont(const Impl_cont &) = delete;
        Impl_cont& operator=(const Impl_cont &) = delete;
        ~Impl_cont() = default;
    };
} // namespace Impl
