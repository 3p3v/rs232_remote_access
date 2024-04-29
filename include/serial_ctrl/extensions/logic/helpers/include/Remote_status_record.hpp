#pragma once

#include <Remote_status.hpp>
#include <Remote_conf_port_rec.hpp>

namespace Logic
{
    class Remote_status_record : virtual public Remote_conf_port_rec
    {
    public:
        /// @brief 
        Remote_status status{Remote_status::Not_connected};

        Remote_status_record(Remote_conf_port conf_port = Remote_conf_port::Configurable);
    }; 
}