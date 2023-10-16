#pragma once

#include <Inter_controller.hpp>
#include <impl\User_set_opt.hpp>
#include <Server.hpp>
// #include <Traffic_mqtt_connect_options.hpp>
// #include <Traffic_mqtt_action_listener.hpp>
// #include <Traffic_mqtt_user.hpp>
// #include <Traffic_mqtt_ssh.hpp>
#include <mqtt/async_client.h>
#include <string>
#include <set>

namespace Mqtt_port
{
    namespace Impl
    {
        template <typename Scallb, typename Rcallb>
        class Controller;

        // template <typename Scallb, typename Rcallb>
        template <typename Scallb, typename Rcallb>
        Controller(Server, User_get_intf, Scallb, Rcallb) -> Controller<Scallb, Rcallb>;

        class Callb_impl : public mqtt::iaction_listener, public Defs
        {
        protected:
            std::shared_ptr<mqtt::async_client> client;

        public:
            Callb_impl(std::shared_ptr<mqtt::async_client> client)
                : client{client}
            {
            }
        };

        class Send_callb_impl : public Callb_impl
        {
            IO_callb &io_callb;
        public:
            Send_callb_impl(std::shared_ptr<mqtt::async_client> client, IO_callb &io_callb)
                : Callb_impl(client), io_callb{io_callb}
            {
            }

            virtual void on_failure(const mqtt::token &asyncActionToken)
            {
                io_callb.fail();
                client->disconnect(time_to_disconnect);
            }

            virtual void on_success(const mqtt::token &asyncActionToken)
            {
            }
        };

        class Channel_conn_callb_impl : public Callb_impl
        {
            std::shared_ptr<Connector> connector;
            C_callb &channel_conn_callback;
        public:
            Channel_conn_callb_impl(std::shared_ptr<mqtt::async_client> client, std::shared_ptr<Connector> connector, C_callb &channel_conn_callback)
                : Callb_impl(client), connector{std::move(connector)}, channel_conn_callback{channel_conn_callback}
            {
            }

            virtual void on_failure(const mqtt::token &asyncActionToken)
            {
                channel_conn_callback.fail();
                client->disconnect(time_to_disconnect);
            }

            virtual void on_success(const mqtt::token &asyncActionToken)
            {
                channel_conn_callback.success();
                if (connector->remaining_channels())
                    client->subscribe(connector->get_channel_to_con(), qos, nullptr, *this);
            }
        };

        class Conn_callb_impl : public Callb_impl
        {
            C_callb &conn_callback;
        public:
            Conn_callb_impl(std::shared_ptr<mqtt::async_client> client, C_callb &conn_callback)
                : Callb_impl(client), conn_callback{conn_callback}
            {
            }

            virtual void on_failure(const mqtt::token &asyncActionToken)
            {
                conn_callback.fail();
                client->disconnect(time_to_disconnect);
            }

            virtual void on_success(const mqtt::token &asyncActionToken)
            {
            }
        };

        template <typename Scallb, typename Rcallb>
        class Controller final : public Basic_controller<Scallb, Rcallb>,
                                 public virtual mqtt::callback,
                                 public Defs
        {
        protected:
            // std::string server_address;

            std::shared_ptr<mqtt::async_client> client;
            mqtt::connect_options options;

            /* Impl specyfic callbacks */
            Send_callb_impl send_callb_impl{client, sent_callb};
            Conn_callb_impl conn_callb_impl{client, conn_callb};
            Channel_conn_callb_impl ch_conn_callb_impl{client, connector, channel_conn_callb};

            void write_handle(const std::string &channel_name, const Data &data, std::size_t write_len)
            {
                auto msg = mqtt::make_message(channel_name, &data.begin(), write_len);
                client.get()->publish(msg, nullptr, send_callb_impl);
            }

            virtual void connected(const std::string & /*cause*/)
            {
                conn_callb.success();
                client->subscribe(connector->get_channel_to_con(), qos, nullptr, ch_conn_callb_impl);
            }

            virtual void connection_lost(const std::string & /*cause*/)
            {
                conn_callb.fail();
            }

            virtual void message_arrived(mqtt::const_message_ptr msg)
            {
                read(msg->get_topic(), Data(msg->get_payload().begin(), msg->get_payload().end()));
                rec_callb.success(msg->get_topic(), msg->get_payload_str().size());
            }

            virtual void delivery_complete(mqtt::delivery_token_ptr token)
            {
                sent_callb.success(token->get_message()->get_topic(),
                                   token->get_message()->get_payload_str().size());
            }

            void run_handle()
            {
                /* Connect to broker */
                client->connect(options, nullptr, conn_callb_impl);
            }

        public:
            Controller(Server &server, User_opt &user, Scallb &&sent_msg, Rcallb &&rec_msg)
                : Basic_controller{std::move(sent_msg), std::move(rec_msg)},
                  client{new mqtt::async_client{server.get_full_address(), user.get_id(), nullptr}}
            {
                client->set_callback(*this);
                /* Set options */
                User_set_opt::set_options(user, options);
            }
            Controller(Controller &&) = default;
            Controller &operator=(Controller &&) = default;
            Controller(Controller &) = delete;
            Controller &operator=(Controller &) = delete;
            ~Controller() = default;

            void disconnect(Time time) override
            {
                client->disconnect(time);
            }

            
        };
    }

}