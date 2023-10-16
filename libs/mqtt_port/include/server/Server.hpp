#pragma once

#include <string>

namespace Mqtt_port
{
    class Server final
    {
        std::string ip;
        std::string port;

    public:
        template <typename S1, typename S2>
        Server(S1 &&ip, S2 &&port)
            : ip{std::forward<S1>(ip)},
              port{std::forward<S2>(port)}
        {
        }
    
        std::string get_ip();
        std::string get_port();
        std::string get_full_address();
    };
}