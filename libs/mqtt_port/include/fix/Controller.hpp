#pragma once

#include <Controller_impl.hpp>
#include <Def.hpp>
#include <string>
#include <User.hpp>

namespace Mqtt_port
{
    
    /* Interface for translation between used library and logic of the server */
    namespace Controller
    {
        class Server;

        template <typename Exe>
        class Callback
        {
            std::string channel_name;
        public:
            // void operator()(bool success, unsigned int err) = 0;
        },

        class Controller
        {
            template <typename C>
            using Callb_ptr = std::unique_ptr<C>;
        public:
            Controller(Server &server, User &user, Callback &&read_call);

            void write(const Data& data, Callback &&call);

            void connect(Callb_ptr<Conn_Callb> &&call);

            void subscribe(const std::string &channel_name, Callback &&call);

            void disconnect(Time time);
        };
        // template <typename Str, typename Call>
        // Console(Str &&server_name, Call &&read_call);

        
    }
}