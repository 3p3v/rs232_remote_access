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
#include <impl/emulated_callbacks/Rec_callb.hpp>
#include <impl/emulated_callbacks/Conn_callb.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        /// @brief Overlay for the poor-designed paho.mqtt.cpp library
        class Controller final : public virtual mqtt::callback
        {
            /* Storing data while connection wasn't established */
            std::atomic_bool is_conn{false};
            Delay_handler<Sub> sub_delay;
            Delay_handler<Pub> pub_delay;

            /* Callbacks for receiving data */
            std::unordered_map<std::string, std::unique_ptr<Rec_callb_intf>> rec_callb;
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

            // disabled, becaurse doesn't work
            void delivery_complete(mqtt::delivery_token_ptr token) override;

        public:
            Controller(Server::Get_cont &&server, User::Get_cont &&user);
            Controller(Controller &&) = default;
            Controller &operator=(Controller &&) = default;
            Controller(Controller &) = delete;
            Controller &operator=(Controller &) = delete;
            ~Controller() = default;

            /// @brief Connect, with callback
            /// @tparam Ok_callb
            /// @tparam Ec_callb
            /// @param ok_callb
            /// @param ec_callb
            template <typename Ok_callb, typename Ec_callb>
            void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            /// @brief Subscribe, with receive data callback and subscribe callback
            /// @tparam Ok_callb
            /// @tparam Ec_callb
            /// @tparam Sub_ok_callb
            /// @tparam Sub_ec_callb
            /// @param channel_name
            /// @param qos
            /// @param ok_callb
            /// @param ec_callb
            /// @param sub_ok_callb
            /// @param sub_ec_callb
            template <typename Ok_callb, typename Ec_callb, typename Sub_ok_callb, typename Sub_ec_callb>
            void subscribe(const std::string &channel_name,
                           unsigned char qos,
                           Ok_callb &&ok_callb,
                           Ec_callb &&ec_callb,
                           Sub_ok_callb &&sub_ok_callb,
                           Sub_ec_callb &&sub_ec_callb);

            /// @brief Subscribe, with receive data callback
            /// @tparam Ok_callb
            /// @tparam Ec_callb
            /// @param channel_name
            /// @param qos
            /// @param ok_callb
            /// @param ec_callb
            template <typename Ok_callb, typename Ec_callb>
            void subscribe(const std::string &channel_name, unsigned char qos, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

            /// @brief Subscribe, without any callbacks
            /// @param channel_name
            /// @param qos
            void subscribe(const std::string &channel_name, unsigned char qos);

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
        };

        template <typename Ok_callb, typename Ec_callb>
        void Controller::connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            /* Connect to broker */
            client->connect(options, nullptr, std::make_unique<Vanilla_callb>(std::move(ok_callb), std::move(ec_callb)));
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
            rec_callb.emplace(channel_name, make_rec_callb{std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb)});
            
            if (!is_conn)
            {
                /* Subscribe later */
                sub_delay.emplace(channel_name, qos, std::make_unique<Vanilla_callb>(std::forward<Sub_ok_callb>(sub_ok_callb), std::forward<Sub_ec_callb>(sub_ec_callb)));
            }
            else
            {
                /* Subscribe */
                client->subscribe(channel_name, qos, nullptr, std::make_unique<Vanilla_callb>(std::forward<Sub_ok_callb>(sub_ok_callb), std::forward<Sub_ec_callb>(sub_ec_callb)));
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
                client->subscribe(channel_name, qos);
            }
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
                client->publish(channel_name, &(*begin), end - begin, qos, false, nullptr, IO_callb{end - begin, std::move(ok_callb), std::move(ec_callb)});
            }
        }
    }

}