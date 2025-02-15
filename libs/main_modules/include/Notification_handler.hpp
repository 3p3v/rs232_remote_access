#pragma once

#include <Notification_manager.hpp>

namespace Logic
{
    class Notification_handler final : public Notification_manager
    {
        bool debug;

    public:
        /// @brief Forward error
        /// @param exc
        void error(Dev_num dev_num, const std::exception &exc) const override;

        /// @brief Forward debug information
        /// @param info
        void debug(Dev_num dev_num, const std::string &info) const override;

        Notification_handler(
            bool close_on_timeout,
            bool close_on_data_loss,
            bool close_on_protocol_error,
            bool debug);
    };

} // namespace Logic
