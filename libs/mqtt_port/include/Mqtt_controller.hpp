#pragma once

#include <Inter_controller.hpp>
// #include <Traffic_mqtt_connect_options.hpp>
#include <Traffic_mqtt_action_listener.hpp>
// #include <Traffic_mqtt_user.hpp>
// #include <Traffic_mqtt_ssh.hpp>
#include <mqtt/async_client.h>
#include <string>
#include <set>

namespace Mqtt_port
{
    class Defs
    {
    protected:
        static constexpr unsigned int time_to_disconnect = 200;
        static constexpr unsigned char qos = 0;
    };
    
    class Callb : public mqtt::iaction_listener, public Defs
    {
    protected:
        std::shared_ptr<mqtt::async_client> client;
        std::shared_ptr<Connector> connector;

    public:
        Callb(std::shared_ptr<mqtt::async_client> client,
              std::shared_ptr<Connector> connector)
            : client{client}, connector{connector}
        {
        }
    };

    class Send_callb : public Callb
    {
    public:
        Send_callb(std::shared_ptr<mqtt::async_client> client,
                   std::shared_ptr<Connector> connector)
            : Callb(client, connector)
        {
        }

        virtual void on_failure(const mqtt::token &asyncActionToken)
        {
            throw std::runtime_error("Could not send message!");
            client->disconnect(time_to_disconnect);
        }

        virtual void on_success(const mqtt::token &asyncActionToken)
        {
        }
    };

    class Channel_conn_callb : public Callb
    {
    public:
        Channel_conn_callb(std::shared_ptr<mqtt::async_client> client,
                   std::shared_ptr<Connector> connector)
            : Callb(client, connector)
        {
        }

        virtual void on_failure(const mqtt::token &asyncActionToken)
        {
            throw std::runtime_error("Could not subscribe to channel!");
            client->disconnect(time_to_disconnect);
        }

        virtual void on_success(const mqtt::token &asyncActionToken)
        {
            if (connector->remaining_channels())
                client->subscribe(connector->get_channel_to_con(), qos, nullptr, *this);
        }
    };

    class Conn_callb : public Callb
    {
    public:
        Conn_callb(std::shared_ptr<mqtt::async_client> client,
                   std::shared_ptr<Connector> connector)
            : Callb(client, connector)
        {
        }

        virtual void on_failure(const mqtt::token &asyncActionToken)
        {
            throw std::runtime_error("Could not connect to broker!");
            client->disconnect(time_to_disconnect);
        }

        virtual void on_success(const mqtt::token &asyncActionToken)
        {
        }
    };

    class Mqtt_controller final : public Basic_controller,
                                  public virtual mqtt::callback, 
                                  public Defs
    {
    protected:
        std::string server_address;

        std::shared_ptr<mqtt::async_client> client;
        mqtt::connect_options options;

        Send_callb send_callb{client, connector};
        Conn_callb conn_callb{client, connector};
        Channel_conn_callb ch_conn_callb{client, connector};

        void write_handle(const std::string &channel_name, const Data &data, std::size_t write_len)
        {
            auto msg = mqtt::make_message(channel_name, &data.begin(), write_len);
            client.get()->publish(msg, nullptr, send_callb);
        }

    public:
        virtual void connected(const std::string & /*cause*/)
        {
            if (!connector->remaining_channels())
                throw std::logic_error("No channels to connect were specyfied!");

            client->subscribe(connector->get_channel_to_con(), qos, nullptr, ch_conn_callb);
        }

        virtual void connection_lost(const std::string & /*cause*/)
        {
            throw std::runtime_error("Connection was lost or could not connect!");
        }

        virtual void message_arrived(mqtt::const_message_ptr msg)
        {
            receiver->read(msg->get_topic(), Data(msg->get_payload().begin(), msg->get_payload().end()));
        }

        virtual void delivery_complete(mqtt::delivery_token_ptr token)
        {
            // callb.get()->write_call(token.get()->get_message().get()->get_topic(),
            //                         token.get()->get_message().get()->get_payload_str().size());
        }

        template <typename Str>
        Mqtt_controller(Str &&server_address)
            : server_address{std::forward<Str>(server_address)},
              client{new mqtt::async_client{server_address, "1", nullptr}}
        {
            client->set_callback(*this);
        }
        Mqtt_controller(Mqtt_controller &&) = default;
        Mqtt_controller &operator=(Mqtt_controller &&) = default;
        Mqtt_controller(Mqtt_controller &) = delete;
        Mqtt_controller &operator=(Mqtt_controller &) = delete;
        ~Mqtt_controller() = default;

        void run_handle()
        {
            /* Set options */
            options.set_clean_session(true);
            options.set_user_name("admin");
            options.set_password("admin");

            /* Connect to broker */
            client->connect(options, nullptr, conn_callb);
        }
    };
}