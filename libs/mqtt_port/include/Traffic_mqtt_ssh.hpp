#pragma once

#include <string>

namespace Mqtt_port
{    
    class Traffic_mqtt_ssh
    {
        std::string key_file;
        std::string cert_file;
        bool ssh{false};

    public:
        Traffic_mqtt_ssh() = default;
        template<typename Str>
        Traffic_mqtt_ssh(Str &&key_file, Str &&cert_file) : key_file{std::forward<Str>(key_file)},
                                                            cert_file{std::forward<Str>(cert_file)},
                                                            ssh{true}
        {
        }
        // Traffic_mqtt_ssh(Traffic_mqtt_ssh &&) = default;
        // Traffic_mqtt_ssh& operator=(Traffic_mqtt_ssh &&) = default;
        // Traffic_mqtt_ssh(Traffic_mqtt_ssh &) = default;
        // Traffic_mqtt_ssh& operator=(Traffic_mqtt_ssh &) = default;
        // ~Traffic_mqtt_ssh() = default;

        bool uses_ssh();
    };
}