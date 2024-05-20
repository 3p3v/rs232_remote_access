#pragma once

#include <unordered_map>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <Device_holder.hpp>
#include <Notyfier_defs.hpp>
#include <Notyfication_forwarder.hpp>

namespace Logic
{
    class Dev_cont
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;
        using Dev_cont_t = std::unordered_map<Notyfier_defs::Dev_num, Device_holder>;
        using iterator = Dev_cont_t::iterator;

        /// @brief 
        Dev_cont_t devs;

        /// @brief 
        Dev_num num{0};

        /// @brief Protect from access to devices  from multiple threads
        std::mutex dev_mutex{};
    };
}