#include <Remote_dev.hpp>
#include <utility>

namespace Logic
{
    Remote_dev::Remote_dev(Remote_conf_port conf_port)
        : Remote_conf_port_rec{conf_port}
    {
    }
    Remote_dev::Remote_dev(Port_settings_storage &&port_settings, Remote_conf_port conf_port)
        : Remote_record{std::move(port_settings), conf_port}
    {
    }
}