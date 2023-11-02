#pragma once

#include <impl/Controller.hpp>
#include <impl/Impl_server.hpp>
#include <impl/Impl_user.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        void Controller::connected(const std::string & /*cause*/)
        {
            is_conn = true;
            conn_callb->success();

            if (!delayed_pub.empty())
            {
                std::for_each(delayed_pub.cbegin(), delayed_pub.cend(),
                              [this](auto &pub)
                              {
                                write(pub.first, pub.second.cbegin(), pub.second.cend());
                              });
            }

            if (!delayed_sub.empty())
            {
                std::for_each(delayed_sub.cbegin(), delayed_sub.cend(),
                              [this](auto &sub)
                              {
                                subscribe(sub.first, sub.second);
                              });
            }
        }

        void Controller::connection_lost(const std::string & /*cause*/)
        {
            is_conn = false;
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

        Controller::Controller(Server::Get_cont &&server,
                               User::Get_cont &&user)
            : client{new mqtt::async_client{"tcp://" + server.get(Server::Option::ip) + ":" + server.get(Server::Option::port), 
                                            user.get(User::Option::id)}}
        {
            client->set_callback(*this);
            /* Set options */
            Impl_user::set_options(std::move(user), options);
            Impl_server::set_options(std::move(server), options);
        }

        void Controller::subscribe(const std::string &channel_name, unsigned char qos)
        {
            if (!is_conn)
            {
                delayed_sub.emplace_back(channel_name, qos);
            }
            else
            {
                /* Subscribe */
                client->subscribe(channel_name, qos, nullptr, ch_conn_callb_impl);
            }
        }

        void Controller::disconnect(Time time)
        {
            client->disconnect(time);
        }
    }

}