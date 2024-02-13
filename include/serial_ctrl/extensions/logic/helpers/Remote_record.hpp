#pragma once

#include <Remote_status.hpp>
#include <Port_settings_storage.hpp>

namespace Logic
{
    class Remote_record
    {
    public:
        Remote_status status{Remote_status::not_connected};
        Port_settings_storage port_settings{};
    }; 
}