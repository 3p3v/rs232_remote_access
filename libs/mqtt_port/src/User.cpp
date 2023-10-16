#include <User.hpp>

namespace Mqtt_port
{
    std::string User::get_username() 
    {
        return username;
    }

    std::string User::get_password() 
    {
        return password;
    }

    std::string User::get_id()
    {
        return id;
    }
}