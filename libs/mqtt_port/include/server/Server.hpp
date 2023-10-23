#pragma once

#include <Server_get_intf.hpp>

namespace Mqtt_port
{
    class Server final : public Server_get_intf
    {
    public:
        std::string ip;
        std::string port;
        std::string crt_file_name;
        std::string pem_file_name;

        Server() = default;

        template <typename S1, typename S2, typename S3, typename S4>
        Server(S1 &&ip, S2 &&port, S3 &&crt_file_name, S4 &&pem_file_name);

        std::string get_ip() override;
        std::string get_port() override;
        std::string get_full_address() override;
        std::string get_crt() override;
        std::string get_pem() override;
    };

    template <typename S1, typename S2, typename S3, typename S4>
    Server::Server(S1 &&ip, S2 &&port, S3 &&crt_file_name, S4 &&pem_file_name)
        : ip{std::forward<S1>(ip)},
          port{std::forward<S2>(port)},
          crt_file_name{std::forward<S3>(crt_file_name)},
          pem_file_name{std::forward<S4>(pem_file_name)}
    {
    }
}