#include <Server.hpp>

namespace Mqtt_port
{
    std::string Server::get_ip()
    {
        return ip;
    }

    std::string Server::get_port()
    {
        return port;
    }

    std::string Server::get_full_address()
    {
        return ip + ":" + port;
    }
}