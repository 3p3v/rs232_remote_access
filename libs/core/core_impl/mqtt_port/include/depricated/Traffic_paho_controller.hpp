#pragma once

#include <Traffic_mqtt_controller.hpp>
#include <Traffic_paho_action_listener.hpp>
#include <Traffic_paho_callback.hpp>
#include <string>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    class Traffic_paho_controller final : public Traffic_mqtt_controller,
                                          public virtual mqtt::callback,
					                      public virtual mqtt::iaction_listener
    {
        mqtt::async_client client;
        mqtt::connect_options options;

    protected:
        void subscribre(const std::string &channel_name) override final;
        void connect_callback(const std::string &cause) override final;
        void subscribre_callback() override final;
        void connect() override;

        virtual void connected(const std::string& /*cause*/);
	    virtual void connection_lost(const std::string& /*cause*/);

    public:
        // template<
        template <typename Str>
        Traffic_paho_controller(Str &&server_address,
                                Traffic_mqtt_user &&user,
                                Traffic_mqtt_ssh &&ssh,
                                Traffic_mqtt_callbacks &&callbacks);

        void run() override;
    };
}