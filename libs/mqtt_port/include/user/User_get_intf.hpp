#pragma once

#include <string>

namespace Mqtt_port
{
    class User_get_intf
    {
    public:
        virtual std::string get_username() = 0;
        virtual std::string get_password() = 0;
        virtual std::string get_id() = 0;
    };
}