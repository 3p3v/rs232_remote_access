#pragma once

#include <Notyfier_defs.hpp>

namespace Logic
{
    /// @brief Interface for removing devices
    class Dev_remover
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;

        /// @brief Remove the device with specyfied number
        /// @param num Device number
        virtual void remove(Dev_num num) = 0;

        Dev_remover() = default;
        Dev_remover(Dev_remover &&) = default;
        Dev_remover& operator=(Dev_remover &&) = default;
        Dev_remover(const Dev_remover &) = default;
        Dev_remover& operator=(const Dev_remover &) = default;
        virtual ~Dev_remover() = default;
    };
} // namespace Logic
