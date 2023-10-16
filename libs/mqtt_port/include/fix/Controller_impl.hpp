#pragma once

#include <Def.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    /* Interface for translation between used library and logic of the server */
    namespace Controller
    {
        class Controller;

        template <typename Call>
        void write(Controller &cont, const Data& data, Call &&call)
        {

        }

        template <typename Call>
        void connect(Controller &cont, Call &&call)
        {

        }

        template <typename Call>
        void subscribe(Controller &cont, const std::string &channel_name, Call &&call)
        {

        }

        void set_user_options(Controller &cont, User &user)
        {

        }

        void disconnect(Controller &cont, Time time)
        {

        }
    };
}