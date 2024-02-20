#pragma once

#include <Remote_status.hpp>

namespace Logic
{
    class Remote_status_record
    {
    public:
        /// @brief 
        Remote_status status{Remote_status::not_connected};
    }; 
}