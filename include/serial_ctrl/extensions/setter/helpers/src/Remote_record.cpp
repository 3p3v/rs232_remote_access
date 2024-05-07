#include <Remote_record.hpp>
#include <utility>

Logic::Remote_record::Remote_record(Remote_conf_port conf_port)
    : Remote_conf_port_rec{conf_port}
{
}

Logic::Remote_record::Remote_record(const Port_settings_storage &port_settings)
    : port_settings{port_settings}
{
}

Logic::Remote_record::Remote_record(Port_settings_storage &&port_settings)
    : port_settings{std::move(port_settings)}
{
}

Logic::Remote_record::Remote_record(const Port_settings_storage &port_settings, Remote_conf_port conf_port)
    : Remote_conf_port_rec{conf_port}, port_settings{port_settings}
{
}
