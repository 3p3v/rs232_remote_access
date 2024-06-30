#pragma once

#include <Notyfier_defs.hpp>

namespace Logic
{
    class Dev_cont;

    /// @brief Base class for Dev_con tinterfaces
    class Dev_ext
    {
    protected:
        /// @brief Container of devices
        Dev_cont &devs;

    public:
        using Dev_num = Notyfier_defs::Dev_num;

        Dev_ext(Dev_cont &devs);
        Dev_ext(Dev_ext&&) = default;
        Dev_ext& operator=(Dev_ext&&) = default;
        Dev_ext(const Dev_ext &) = default;
        Dev_ext& operator=(const Dev_ext &) = default;
        virtual ~Dev_ext() = 0;
    };
}