#include <Proto_rec.hpp>

Logic::Proto_rec::Proto_rec(
    Remote_conf_port conf_port,
    bool settings_known,
    Port_settings_rec &&port_settings)
    : conf_port{conf_port},
      settings_known{settings_known},
      port_settings{std::move(port_settings)}
{
}