#pragma once

#include <Notification_manager.hpp>

namespace Logic
{
    class Notifier_manager_impl final : public Notification_manager
    {
        bool debug;

    public:
        /// @brief Forward error 
        /// @param exc 
        void error(Dev_num dev_num, const std::exception &exc) const override;

        /// @brief Forward debug information
        /// @param info 
        void debug(Dev_num dev_num, const std::string &info) const override;

        Notifier_manager_impl(bool debug);
    };
}