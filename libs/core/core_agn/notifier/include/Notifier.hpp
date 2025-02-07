#pragma once

#include <Notification_manager.hpp>

namespace Logic
{
    class Notifier final
    {
    private:
        Notification_manager::Dev_num dev_num;

        const Notification_manager &manager;

    public:
        /// @brief Forward error 
        /// @param exc 
        void error(const std::exception &exc) const;

        /// @brief Forward debug information
        /// @param info 
        void debug(const std::string &info) const;

        Notifier(Notification_manager::Dev_num dev_num, const Notification_manager &manager);
    };
}