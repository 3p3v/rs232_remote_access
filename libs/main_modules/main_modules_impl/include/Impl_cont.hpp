#pragma once

#include <unordered_map>
#include <mutex>
#include <Notyfier_defs.hpp>
#include <Dev_info.hpp>
#include <Dev_cont.hpp>

namespace Impl
{
    /// @brief Contains all devices and their information (MAC, port, ect.)
    class Impl_cont : public Dev_cont
    {
    public:
        using Dev_num = ::Logic::Notyfier_defs::Dev_num;
        using Info_num = Dev_num;
        using Info_cont_t = std::unordered_map<Dev_num, std::shared_ptr<Dev_info>>;
        using iterator = Info_cont_t::iterator;
        using const_iterator = Info_cont_t::const_iterator;

        /// @brief Additional info about devices
        Info_cont_t infos;

        Impl_cont() = default;
        Impl_cont(Impl_cont &&);
        Impl_cont& operator=(Impl_cont &&);
        Impl_cont(const Impl_cont &) = delete;
        Impl_cont& operator=(const Impl_cont &) = delete;
        ~Impl_cont() = default;
    };
} // namespace Impl
