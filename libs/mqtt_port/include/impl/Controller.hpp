#pragma once

// #include <Inter_controller.hpp>
#include <User.hpp>
#include <impl\User_set_opt.hpp>
#include <User.hpp>
#include <Server.hpp>
// #include <Traffic_mqtt_connect_options.hpp>
// #include <Traffic_mqtt_action_listener.hpp>
// #include <Traffic_mqtt_user.hpp>
// #include <Traffic_mqtt_ssh.hpp>
#include <mqtt/async_client.h>
#include <Callbacks.hpp>
#include <Def.hpp>
#include <string>
#include <set>

namespace Mqtt_port
{
    namespace Impl
    {
        // template <typename Scallb, typename Rcallb>
        // class Controller;

        // template <typename Scallb, typename Rcallb>
        // template <typename Scallb, typename Rcallb>
        // Controller(Server, User_get_intf, Scallb, Rcallb) -> Controller<Scallb, Rcallb>;
        
        template <typename Callb_t>
        class Callb_impl : public mqtt::iaction_listener, public Defs
        {
        protected:
            using Grip = std::unique_ptr<Callb_t>;

        private:
            Grip &callb;

        protected:
            Callb_t& get_callb()
            {
                return *callb;
            }

        public:
            Callb_impl(Grip &callb)
                : callb{callb}
            {
            }
        };

        class Send_callb_impl : public Callb_impl<Mqtt_port::O_callb>
        {
        public:
            Send_callb_impl(Grip &io_callb)
                : Callb_impl<O_callb>{io_callb}
            {
            }

            virtual void on_failure(const mqtt::token &asyncActionToken)
            {
                get_callb().fail();
                // client->disconnect(time_to_disconnect);
            }

            virtual void on_success(const mqtt::token &asyncActionToken)
            {
            }
        };

        class Channel_conn_callb_impl : public Callb_impl<C_callb>
        {
        public:
            Channel_conn_callb_impl(Grip &channel_conn_callback)
                : Callb_impl<C_callb>{channel_conn_callback}
            {
            }

            virtual void on_failure(const mqtt::token &asyncActionToken)
            {
                get_callb().fail();
                // client->disconnect(time_to_disconnect);
            }

            virtual void on_success(const mqtt::token &asyncActionToken)
            {
                get_callb().success();
                // if (connector->remaining_channels())
                //     client->subscribe(connector->get_channel_to_con(), qos, nullptr, *this);
            }
        };

        class Conn_callb_impl : public Callb_impl<C_callb>
        {
        public:
            Conn_callb_impl(Grip &conn_callback)
                : Callb_impl<C_callb>{conn_callback}
            {
            }

            virtual void on_failure(const mqtt::token &asyncActionToken)
            {
                get_callb().fail();
                // client->disconnect(time_to_disconnect);
            }

            virtual void on_success(const mqtt::token &asyncActionToken)
            {
            }
        };

        // template <typename Scallb, typename Rcallb>
        class Controller final : //public Basic_controller<Scallb, Rcallb>,
                                 public virtual mqtt::callback,
                                 public Defs
        {
        protected:
            // std::string server_address;

            std::shared_ptr<mqtt::async_client> client;
            mqtt::connect_options options;

            /* Impl specyfic callbacks */
            std::unique_ptr<O_callb> sent_callb;
            std::unique_ptr<I_callb> rec_callb;
            std::unique_ptr<C_callb> conn_callb;
            std::unique_ptr<C_callb> channel_conn_callb;
            Send_callb_impl send_callb_impl{sent_callb};
            Conn_callb_impl conn_callb_impl{conn_callb};
            Channel_conn_callb_impl ch_conn_callb_impl{channel_conn_callb};

            virtual void connected(const std::string & /*cause*/)
            {
                conn_callb->success();
                // client->subscribe(connector->get_channel_to_con(), qos, nullptr, ch_conn_callb_impl);
            }

            virtual void connection_lost(const std::string & /*cause*/)
            {
                conn_callb->fail();
            }

            virtual void message_arrived(mqtt::const_message_ptr msg)
            {
                // read(msg->get_topic(), Data(msg->get_payload().begin(), msg->get_payload().end()));
                rec_callb->success(msg->get_topic(), Data(msg->get_payload().begin(), msg->get_payload().end()));
            }

            virtual void delivery_complete(mqtt::delivery_token_ptr token)
            {
                sent_callb->success(token->get_message()->get_topic(),
                                   token->get_message()->get_payload_str().size());
            }


        public:
            Controller(Server &server, 
                       User_opt &user)
                    //    IO_callb &&rec_callb)
                : client{new mqtt::async_client{server.get_full_address(), user.get_id(), nullptr}}
                //   rec_callb{std::make_unique<IO_callb>(std::move(rec_callb))}
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

            template <typename Callb_t>
            void connect(Callb_t &&conn_callb)
            {
                /* Change callback */
                this->conn_callb.reset(new Callb_t{std::move(conn_callb)});
                
                /* Connect to broker */
                client->connect(options, nullptr, conn_callb_impl);
            }

            template <typename Callb_t>
            void subscribe(const std::string &channel_name, unsigned char qos, Callb_t &&channel_conn_callb)
            {
                /* Change callback */
                this->channel_conn_callb.reset(new Callb_t{std::move(channel_conn_callb)});

                /* Subscribe */
                client->subscribe(channel_name, qos, nullptr, ch_conn_callb_impl);
            }

            void subscribe(const std::string &channel_name, unsigned char qos)
            {
                /* Subscribe */
                client->subscribe(channel_name, qos, nullptr, ch_conn_callb_impl);
            }

            void disconnect(Time time)
            {
                client->disconnect(time);
            }

            /* Write data, use previous callback */
            void write(const std::string &channel_name, const Data &data, std::size_t write_len)
            {
                /* Send message */
                auto msg = mqtt::make_message(channel_name, &data.begin(), write_len);
                client.get()->publish(msg, nullptr, send_callb_impl);
            }

            /* Set callback and write data */
            template <typename Callb_t>
            void write(const std::string &channel_name, const Data &data, std::size_t write_len, Callb_t &&sent_callb)
            {
                /* Change callback */
                this->sent_callb.reset(new Callb_t{std::move(sent_callb)});

                /* Send message */
                auto msg = mqtt::make_message(channel_name, &data.begin(), write_len);
                client.get()->publish(msg, nullptr, send_callb_impl);
            }

            /* Set write callack */
            template <typename Callb_t>
            void set_write_callb(Callb_t &&sent_callb)
            {
                /* Change callback */
                this->sent_callb.reset(new Callb_t{std::move(sent_callb)});
            }

            /* Set write callack */
            template <typename Callb_t>
            void set_read_callb(Callb_t &&rec_callb)
            {
                /* Change callback */
                this->rec_callb.reset(new Callb_t{std::move(rec_callb)});
            }

            
        };
    }

}