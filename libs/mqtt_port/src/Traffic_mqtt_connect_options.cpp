#include <Traffic_mqtt_connect_options.hpp>
#include <set>

namespace Mqtt_port
{
    void Traffic_mqtt_connect_usr_options::add_option(Option option)
    {
        options.emplace(option);
    }

    Traffic_mqtt_connect_usr_options::Options_cont_type &Traffic_mqtt_connect_usr_options::get_options()
    {
        return options;
    }

}