#pragma once

#include <impl/Controller.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        void Controller::connected(const std::string & /*cause*/)
        {
            conn_callb->success();
        }

        void Controller::connection_lost(const std::string & /*cause*/)
        {
            conn_callb->fail();
        }

        void Controller::message_arrived(mqtt::const_message_ptr msg)
        {
            rec_callb->success(msg->get_topic(), msg->get_payload().begin(), msg->get_payload().end());
        }

        // Temporarly disabled
        void Controller::delivery_complete(mqtt::delivery_token_ptr token)
        {
            // sent_callb->success(token->get_message()->get_topic(),
            //                     token->get_message()->get_payload_str().size());
        }

        Controller::Controller(Server &server,
                               User_opt &user)
            : client{new mqtt::async_client{server.get(Server::Options::ip) + server.get(Server::Options::port), 
                                            user.get(User::Options::id), nullptr}}
        {
            client->set_callback(*this);
            /* Set options */
            Impl_user::set_options(user, options);
            Impl_server::set_options(server, options);
        }

        void Controller::subscribe(const std::string &channel_name, unsigned char qos)
        {
            /* Subscribe */
            client->subscribe(channel_name, qos, nullptr, ch_conn_callb_impl);
        }

        void Controller::disconnect(Time time)
        {
            client->disconnect(time);
        }
    }

}