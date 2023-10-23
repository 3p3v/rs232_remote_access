#pragma once

#include <string>

namespace Mqtt_port
{
    class User_get_intf
    {
    public:
        User_get_intf() = default;
        User_get_intf(User_get_intf&) = default;
        User_get_intf& operator=(User_get_intf&) = default;
        User_get_intf(User_get_intf&&) = default;
        User_get_intf& operator=(User_get_intf&&) = default;

        virtual std::string get_username() = 0;
        virtual std::string get_password() = 0;
        virtual std::string get_id() = 0;
    };
}