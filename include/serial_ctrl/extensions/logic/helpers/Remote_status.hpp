#pragma once

namespace Logic
{
    enum class Remote_status
    {
        /// @brief Not tried to connect yet
        not_connected,
        /// @brief Choosing port parameters
        establishing_parameters,
        /// @brief Ready to exchange data
        data_exchange,
        /// @brief Got disconnected 
        disconnected
    };
}