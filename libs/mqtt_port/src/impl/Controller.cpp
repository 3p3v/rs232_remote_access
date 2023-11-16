#pragma once

#include <impl/Controller.hpp>
#include <impl/Impl_server.hpp>
#include <impl/Impl_user.hpp>
#include <Controller.hpp>
// #include <Rec_helper.hpp>

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
                // /* Send overdue messages */
                // pub_delay.for_each([this](auto &&pub)
                //                    { 
                //                         if (pub.util_callb())
                //                         {
                //                             client->publish(pub.channel_name, 
                //                                             &(*pub.data->begin()), 
                //                                             pub.data->end() - pub.data->begin(), 
                //                                             pub.qos, 
                //                                             false, 
                //                                             nullptr, 
                //                                             std::move(pub.callb));
                //                         }
                //                         else
                //                         {
                //                             client->publish(pub.channel_name, 
                //                                             &(*pub.data->begin()),
                //                                             pub.data->end() - pub.data->begin(), 
                //                                             pub.qos, 
                //                                             false);
                //                         } });
            }

            if (!sub_delay.empty())
            {
                sub_delay.for_each([this](auto &sub)
                                   { 
                                    if (sub.util_callb())
                                        {
                                            client->subscribe(sub.channel_name, 
                                                              sub.qos, 
                                                              nullptr, 
                                                              std::move(sub.callb)
                                                              mqtt::subscribe_options{true});
                                        }
                                        else
                                        {
                                            client->subscribe(sub.channel_name, 
                                                            sub.qos,
                                                            mqtt::subscribe_options{true});
                                        } });
            }
        }

        void Controller::connection_lost(const std::string &cause)
        {
            is_conn = false;
            // TODO translate string to int, doesn't matter really, paho returns empty string...
            conn_callb->ec_callb(-1);
        }

        void Controller::message_arrived(mqtt::const_message_ptr msg)
        {
            /* Find topic's callback */
            if (rec_callb.find(msg->get_topic()) != rec_callb.end())
            {
                // Move entire message so data don't get deallocated
                rec_callb[msg->get_topic()]->ok_callb(std::move(msg));
            }
        }

        void Controller::delivery_complete(mqtt::delivery_token_ptr token)
        {
        }

        Controller::Controller(Server::Get_cont &&server,
                               User::Get_cont &&user)
        {
            client = std::make_shared<mqtt::async_client>("tcp://" + server.get(Server::Option::ip) + ":" + server.get(Server::Option::port),
                                                          user.get(User::Option::id),
                                                          mqtt::create_options(MQTTVERSION_5));
            
            client->set_callback(*this);


            /* Set options */
            options.set_mqtt_version(MQTTVERSION_5);
            // mqtt::properties properties{mqtt::property{mqtt::property::MAXIMUM_PACKET_SIZE, 65536},
            //                             mqtt::property{mqtt::property::TOPIC_ALIAS_MAXIMUM, 30}};
            // options.set_properties(properties);
            Impl_user::set_options(std::move(user), options);
            Impl_server::set_options(std::move(server), options);
        }

        void Controller::subscribe(const std::string &channel_name, unsigned char qos)
        {
            if (!is_conn)
            {
                sub_delay.emplace(std::move(channel_name), qos);
            }
            else
            {
                /* Subscribe */
                client->subscribe(std::move(channel_name), qos);
            }
        }

        void Controller::disconnect(Time time) 
        {
            client->disconnect(time);
        }

        void Controller::unsubscribe(const std::string &channel_name)
        {
            client->unsubscribe(channel_name);
        }
    }
}
