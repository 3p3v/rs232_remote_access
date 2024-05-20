#pragma once

#include <Notyfier_defs.hpp>
#include <Notyfication_forwarder.hpp>

namespace Logic
{
    class Notyfier final : Notyfication_forwarder
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;

    private:
        Dev_num dev;

    public:
        /// @brief Forward error 
        /// @param exc 
        void error(const std::exception &exc);

        /// @brief Forward debug information
        /// @param info 
        void debug(const std::string &info);

        Notyfier(Dev_num dev, const Notyfication_forwarder &forwarder);
        Notyfier(Dev_num dev, Worker_storage &ws);
    };
}