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

    std::string Server::get_crt()
    {
        return crt_file_name;
    }

    std::string Server::get_pem()
    {
        return pem_file_name;
    }
}