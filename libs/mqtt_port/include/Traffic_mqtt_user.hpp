#pragma once

#include <Traffic_mqtt_connect_options.hpp>
#include <string>

namespace Mqtt_port
{
    class Traffic_mqtt_user final : public Traffic_mqtt_connect_usr_options
    {
        std::string id;
        std::string username;
        std::string password;

    public:
        template<typename Str>
        void set_id(Str &&id)
        {
            this->id = std::forward<Str>(id);
            add_option(Traffic_mqtt_connect_options::option::id);
        }

        template<typename Str>
        void set_username(Str &&username)
        {
            this->username = std::forward<Str>(username);
            add_option(Traffic_mqtt_connect_options::option::username);
        }

        template<typename Str>
        void set_password(Str &&password)
        {
            this->password = std::forward<Str>(password);
            add_option(Traffic_mqtt_connect_options::option::password);
        }
    };
}