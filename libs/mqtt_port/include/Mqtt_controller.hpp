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
    // template <typename Call_t>
    class Mqtt_controller final : public Basic_controller,
                            // public virtual Call_t,
                            public virtual mqtt::callback,
                            public virtual mqtt::iaction_listener
    {
    protected:
        std::string server_address;

        std::shared_ptr<mqtt::async_client> client;
        mqtt::connect_options options;

        Traffic_mqtt_action_listener send_call;
        Traffic_mqtt_action_listener rec_call;

        void conn_handle(const std::string channel_name)
        {
            client.get()->connect(options, nullptr, *this);
        }

        void write_handle(const std::string &channel_name, const Data &data, std::size_t write_len)
        {
            client.get()->publish(channel_name, &data.begin(), write_len);
        }

        virtual void connected(const std::string & /*cause*/)
        {
            callb.get()->conn_call();
            // if (load_channels(get_channels()))
            //     client.subscribe(get_channel_to_con(), 0, nullptr, *this);
        }

        virtual void connection_lost(const std::string & /*cause*/)
        {
            callb.get()->conn_err_call();
        }

        virtual void message_arrived(mqtt::const_message_ptr msg)
        {
            callb.get()->write_call(msg.get()->get_topic(),
                                    msg.get()->get_payload_str().size());
        }

        virtual void delivery_complete(mqtt::delivery_token_ptr token)
        {
            callb.get()->write_call(token.get()->get_message().get()->get_topic(), 
                                    token.get()->get_message().get()->get_payload_str().size());
        }

        virtual void on_failure(const mqtt::token &asyncActionToken)
        {
            callb.get()->conn_err_call();
        }

        virtual void on_success(const mqtt::token &asyncActionToken)
        {
            callb.get()->conn_call();
        }

    public:
        template <typename Str, typename Callb_t>
        Mqtt_controller(Str &&server_address, Callb_t &&callb) 
            : Basic_controller{std::make_shared<Validator>(), std::forward<Callb_t>(callb)},
              server_address{std::forward<Str>(server_address)} 
        {
        }
        Mqtt_controller(Mqtt_controller &&) = default;
        Mqtt_controller &operator=(Mqtt_controller &&) = default;
        Mqtt_controller(Mqtt_controller &) = delete;
        Mqtt_controller &operator=(Mqtt_controller &) = delete;
        ~Mqtt_controller() = default;

        void run_exec()
        {
            options.set_clean_session(true);
            options.set_user_name("admin");
            options.set_password("admin");
            client.get()->set_callback(*this);

            // client.connect(options, nullptr, con_call);
        }
    };
}