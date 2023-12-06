#pragma once

#include <string>
#include <atomic>
#include <set>
#include <unordered_map>
#include <Def.hpp>
#include <User.hpp>
#include <Server.hpp>
#include <Sub.hpp>
#include <Pub.hpp>
#include <Delay_handler.hpp>
#include <IO_callb.hpp>
#include <IO_callb_data.hpp>
#include <Vanilla_callb.hpp>
#include <Unsub_callb.hpp>
#include <impl/emulated_callbacks/Rec_callb.hpp>
#include <impl/emulated_callbacks/Conn_callb.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        /// @brief An overlay for the poor-designed paho.mqtt.cpp library
        class Controller final : public virtual mqtt::callback
        {
        public:
            using Rec_callb_cont = std::unordered_map<std::string, std::unique_ptr<Rec_callb_intf>>;

        private:
            /* Storing data while connection wasn't established */
            std::atomic_bool is_conn{false};
            Delay_handler<Sub> sub_delay;
            Delay_handler<Pub> pub_delay; //TODO enable

            /* Callbacks for receiving data */
            Rec_callb_cont rec_callb;
            /* Callback for connecting to broker */
            std::unique_ptr<Conn_callb_intf> conn_callb;

        public:
            using Data = std::string;

        protected:
            std::shared_ptr<mqtt::async_client> client;
            mqtt::connect_options options;

            void connected(const std::string & /*cause*/) override;

            void connection_lost(const std::string & /*cause*/) override;

            void message_arrived(mqtt::const_message_ptr msg) override;

            // disabled, becaurse doesn't work (disabled in paho...)
            void delivery_complete(mqtt::delivery_token_ptr token) override;

        public:
            Controller(Server::Get_cont &&server, User::Get_cont &&user);
            Controller(Controller &&) = default;
            Controller &operator=(Controller &&) = default;
            Controller(Controller &) = delete;
            Controller &operator=(Controller &) = delete;
            ~Controller() = default;

            /// @brief Connect, with callback
            /// @tparam Ok_callb Functor with no parameters
            /// @tparam Ec_callb Functor with parameters: (int)
            /// @tparam Conn_ec_callb Functor with parameters: (int)
            /// @param ok_callb Callback used when client gets connected
            /// @param ec_callb Callback used when client gets disconnected
            /// @param conn_ec_callb Callback used when connecting failed
            template <typename Ok_callb, typename Ec_callb, typename Conn_ec_callb>
            void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb, Conn_ec_callb &&conn_ec_callb);

            /// @brief Subscribe, with receive data callback and subscribe callback
            /// @tparam Ok_callb Functor with parameters: (const std::string::const_iterator, const std::string::const_iterator)
            /// @tparam Ec_callb Functor with parameters: (int)
            /// @tparam Sub_ok_callb Functor with no parameters
            /// @tparam Sub_ec_callb Functor with parameters: (int)
            /// @param channel_name Topic name
            /// @param qos Quality of service
            /// @param ok_callb Callback used when getting new message
            /// @param ec_callb Callback used when getting new message failed
            /// @param sub_ok_callb Callback used when subscribe was successful
            /// @param sub_ec_callb Callback used when subscribe failed
            template <typename Ok_callb, typename Ec_callb, typename Sub_ok_callb, typename Sub_ec_callb>
            void subscribe(const std::string &channel_name,
                           unsigned char qos,
                           Ok_callb &&ok_callb,
                           Ec_callb &&ec_callb,
                           Sub_ok_callb &&sub_ok_callb,
                           Sub_ec_callb &&sub_ec_callb);

            /// @brief Subscribe, with receive data callback
            /// @tparam Ok_callb Functor with parameters: (const std::string::const_iterator, const std::string::const_iterator)
            /// @tparam Ec_callb Functor with parameters: (int)
            /// @param channel_name Topic name
            /// @param qos Quality of service
            /// @param ok_callb Callback used when getting new message
            /// @param ec_callb Callback used when getting new message failed
            template <typename Ok_callb, typename Ec_callb>
            void subscribe(const std::string &channel_name, unsigned char qos, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            /// @brief Subscribe, without any callbacks
            /// @param channel_name Topic name
            /// @param qos Quality of service
            void subscribe(const std::string &channel_name, unsigned char qos);

            /// @brief Unsubscribe, with callback
            /// @tparam Ok_callb Functor with no parameters
            /// @tparam Ec_callb Functor with parameters: (int)
            /// @param channel_name Topic name
            /// @param ok_callb Callback used when unsubscribe was successful
            /// @param ec_callb Callback used when unsubscribe failed
            template <typename Ok_callb, typename Ec_callb>
            void unsubscribe(const std::string &channel_name, Ok_callb &&ok_callb, Ec_callb &&ec_callb);
            
            /// @brief Unsubscribe, without callback
            /// @param channel_name Topic name
            void unsubscribe(const std::string &channel_name);

            void disconnect(Time time);

            /// @brief Write data, without callback
            /// @tparam Iter
            /// @param channel_name
            /// @param qos
            /// @param begin
            /// @param end
            template <typename Iter>
            void write(const std::string &channel_name, const unsigned char qos, const Iter begin, const Iter end);

            /// @brief Write data, with callback
            /// @tparam Iter
            /// @tparam Ok_callb
            /// @tparam Ec_callb
            /// @param channel_name
            /// @param qos
            /// @param begin
            /// @param end
            /// @param ok_callb
            /// @param ec_callb
            template <typename Iter, typename Ok_callb, typename Ec_callb>
            void write(const std::string &channel_name, const unsigned char qos, const Iter begin, const Iter end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);
        
            // template <typename Iter, typename S1, typename S2>
            // void Controller::write(const std::string &channel_name, const unsigned char qos, S1 &&key, S2 &&val, const Iter begin, const Iter end);

            template <typename Iter, typename S1, typename S2, typename Ok_callb, typename Ec_callb>
            void write(const std::string &channel_name, const unsigned char qos, S1 &&key, S2 &&val, const Iter begin, const Iter end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);
        };

        template <typename Ok_callb, typename Ec_callb, typename Conn_ec_callb>
        void Controller::connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb, Conn_ec_callb &&conn_ec_callb)
        {
            /* Connect to broker */
            client->connect(options, nullptr, make_vanilla_callb([](){}, std::forward<Conn_ec_callb>(conn_ec_callb)));
            conn_callb = make_conn_callb(std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb));
        }

        template <typename Ok_callb, typename Ec_callb, typename Sub_ok_callb, typename Sub_ec_callb>
        void Controller::subscribe(const std::string &channel_name,
                       unsigned char qos,
                       Ok_callb &&ok_callb,
                       Ec_callb &&ec_callb,
                       Sub_ok_callb &&sub_ok_callb,
                       Sub_ec_callb &&sub_ec_callb)
        {
            /* Save data receive callback */
            rec_callb.emplace(channel_name, make_rec_callb(std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb)));
            
            if (!is_conn)
            {
                /* Subscribe later */
                sub_delay.emplace(channel_name, qos, make_vanilla_callb(std::forward<Sub_ok_callb>(sub_ok_callb), std::forward<Sub_ec_callb>(sub_ec_callb)));
            }
            else
            {
                /* Subscribe */
                client->subscribe(channel_name, 
                                  qos, 
                                  nullptr, 
                                  make_vanilla_callb(std::forward<Sub_ok_callb>(sub_ok_callb), std::forward<Sub_ec_callb>(sub_ec_callb)),
                                  mqtt::subscribe_options{true});
            }
        }

        template <typename Ok_callb, typename Ec_callb>
        void Controller::subscribe(const std::string &channel_name, unsigned char qos, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            /* Save data receive callback */
            rec_callb.emplace(channel_name, make_rec_callb(std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb)));
            
            if (!is_conn)
            {
                /* Subscribe later */
                sub_delay.emplace(channel_name, qos);
            }
            else
            {
                /* Subscribe */
                client->subscribe(channel_name, qos, mqtt::subscribe_options{true});
            }
        }

        template <typename Ok_callb, typename Ec_callb>
        void Controller::unsubscribe(const std::string &channel_name, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            /* Unsubscribe topic */
            client->unsubscribe(channel_name, nullptr, make_unsub_callb(rec_callb, channel_name, std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb)));
        }

        template <typename Iter>
        void Controller::write(const std::string &channel_name, const unsigned char qos, const Iter begin, const Iter end)
        {
            if (!is_conn)
            {
                // auto data = std::make_shared<std::string>(begin, end);

                // pub_delay.emplace(channel_name, qos, data);//, std::unique_ptr<mqtt::iaction_listener>{new IO_callb_data{data, data->size(), [](size_t){}, [](int){}}});
            }
            else
            {
                /* Send message */
                client->publish(channel_name, &(*begin), end - begin, qos, false);
            }
        }

        // template <typename Iter, typename S1, typename S2>
        // void Controller::write(const std::string &channel_name, const unsigned char qos, S1 &&key, S2 &&val, const Iter begin, const Iter end)
        // {
        //     if (!is_conn)
        //     {
        //         // auto data = std::make_shared<std::string>(begin, end);

        //         // pub_delay.emplace(channel_name, qos, data);//, std::unique_ptr<mqtt::iaction_listener>{new IO_callb_data{data, data->size(), [](size_t){}, [](int){}}});
        //     }
        //     else
        //     {
        //         mqtt::message_ptr msg_ptr = std::make_shared<mqtt::message>(channel_name, &(*begin), end - begin, qos, false);
        //         mqtt::properties prop{mqtt::property{MQTTPROPERTY_CODE_USER_PROPERTY, std::forward(key), std::forward(val)}};
        //         msg_ptr->set_properties(std::move(prop));
        //         /* Send message */
        //         client->publish(std::move(msg_ptr));
        //     }
        // }

        template <typename Iter, typename Ok_callb, typename Ec_callb>
        void Controller::write(const std::string &channel_name, const unsigned char qos, const Iter begin, const Iter end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            if (!is_conn)
            {
                // auto data = std::make_shared<std::string>(begin, end);

                // pub_delay.emplace(channel_name, qos, data, std::make_unique<IO_callb_data>(data, data.size(), std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb)));
            }
            else
            {
                /* Send message */
                client->publish(channel_name, &(*begin), end - begin, qos, false, nullptr, make_io_callb(end - begin, std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb)));
            }
        }

        template <typename Iter, typename S1, typename S2, typename Ok_callb, typename Ec_callb>
        void Controller::write(const std::string &channel_name, const unsigned char qos, S1 &&key, S2 &&val, const Iter begin, const Iter end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            if (!is_conn)
            {
                // auto data = std::make_shared<std::string>(begin, end);

                // pub_delay.emplace(channel_name, qos, data, std::make_unique<IO_callb_data>(data, data.size(), std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb)));
            }
            else
            {
                mqtt::property p(mqtt::property::code::USER_PROPERTY, std::forward<S1>(key), std::forward<S2>(val));
                mqtt::properties prop{std::move(p)};
                mqtt::message_ptr msg_ptr = std::make_shared<mqtt::message>(channel_name, &(*begin), end - begin, qos, false, prop);
                msg_ptr->set_properties(std::move(prop));
                /* Send message */
                client->publish(std::move(msg_ptr), nullptr, make_io_callb(end - begin, std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb)));
            }
        }
    }

}