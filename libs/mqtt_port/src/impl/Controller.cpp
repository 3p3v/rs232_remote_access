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
            conn_callb->ok_callb();

            if (!pub_delay.empty())
            {
                /* Send overdue messages */
                pub_delay.for_each([this](auto &&pub)
                                   { 
                                        if (pub->util_callb())
                                        {
                                            client->publish(pub->channel_name, 
                                                            &(*pub->data.begin()), 
                                                            pub->data.end() - pub->data.begin(), 
                                                            pub->qos, 
                                                            false, 
                                                            nullptr, 
                                                            std::move(pub->callb));
                                        }
                                        else
                                        {
                                            client->publish(pub->channel_name, 
                                                            &(*pub->data.begin()),
                                                            pub->data.end() - pub->data.begin(), 
                                                            pub->qos, 
                                                            false);
                                        } });
            }

            if (!sub_delay.empty())
            {
                sub_delay.for_each([this](auto &sub)
                                   { 
                                    if (pub->util_callb())
                                        {
                                            client->subscribe(pub->channel_name, 
                                                              pub->qos, 
                                                              nullptr, 
                                                              std::move(pub->callb));
                                        }
                                        else
                                        {
                                            client->subscribe(pub->channel_name, 
                                                            pub->qos);
                                        } });
            }
        }

        void Controller::connection_lost(const std::string &cause)
        {
            is_conn = false;
            conn_callb->ec_callb(-1, cause);
        }

        void Controller::message_arrived(mqtt::const_message_ptr msg)
        {
            /* Find topic's callback */
            if (rec_callb.find(msg->get_topic()) != rec_callb.end())
                rec_callb[msg->get_topic()]->ok_callb(msg->get_payload().cbegin(), msg->get_payload().cend());
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
                sub_delay.emplace(channel_name, qos);
            }
            else
            {
                /* Subscribe */
                client->subscribe(channel_name, qos);
            }
        }

        void Controller::disconnect(Time time)
        {
            client->disconnect(time);
        }
    }

}