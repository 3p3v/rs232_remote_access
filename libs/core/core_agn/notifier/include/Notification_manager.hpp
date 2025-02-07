#pragma once

#include <exception>
#include <string>

namespace Logic
{
    class Notification_manager
    {
    public:
        using Dev_num = unsigned int;

    private:
        unsigned int dev;

    public:
        /// @brief Forward error 
        /// @param exc 
        virtual void error(Dev_num dev_num, const std::exception &exc) const = 0;

        /// @brief Forward debug information
        /// @param info 
        virtual void debug(Dev_num dev_num, const std::string &info) const = 0;
    };
}