#pragma once

#include <string>

class Server_get_intf
{
public:
    Server_get_intf() = default;
    Server_get_intf(Server_get_intf&) = default;
    Server_get_intf& operator=(Server_get_intf&) = default;
    Server_get_intf(Server_get_intf&&) = default;
    Server_get_intf& operator=(Server_get_intf&&) = default;
    virtual ~Server_get_intf() = default;

    virtual std::string get_ip() = 0;
    virtual std::string get_port() = 0;
    virtual std::string get_full_address() = 0;
    virtual std::string get_crt() = 0;
    virtual std::string get_pem() = 0;
};
