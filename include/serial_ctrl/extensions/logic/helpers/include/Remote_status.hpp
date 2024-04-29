#pragma once

namespace Logic
{
    enum class Remote_status
    {
        /// @brief Not tried to connect yet
        Not_connected,
        /// @brief Choosing port parameters
        Establishing_parameters,
        /// @brief Ready to exchange data
        Data_exchange,
        /// @brief Got disconnected 
        Disconnected
    };
}