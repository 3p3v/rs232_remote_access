#pragma once

#include <Traffic_controller.hpp>
#include <Traffic_mqtt_connect_options.hpp>
#include <Traffic_mqtt_user.hpp>
#include <Traffic_mqtt_ssh.hpp>
#include <string>
#include <set>

namespace Mqtt_port
{
    class Traffic_mqtt_controller : public Traffic_controller
    {
        Traffic_mqtt_controller(Traffic_mqtt_controller &) = delete;
        Traffic_mqtt_controller &operator=(Traffic_mqtt_controller &) = delete;

    protected:
        std::string server_address;
        Traffic_mqtt_user user;
        Traffic_mqtt_ssh ssh;

        virtual void connect() = 0;
        virtual void subscribre(const std::string &channel_name) = 0;
        // virtual void subscribre_exec(const string & channel_name) = 0;
        virtual void connect_usr_callback(const std::string &cause) = 0;
        virtual void connect_callback(const std::string &cause) = 0;
        virtual void subscribre_callback() = 0;
        virtual void subscribre_usr_callback() = 0;
        virtual void connect_err_usr_callback(const std::string &cause) = 0;
        virtual void subscribre_err_usr_callback(const int code) = 0;

    public:
        template <typename Str>
        Traffic_mqtt_controller(Str &&server_address,
                                Traffic_mqtt_user &&user,
                                Traffic_mqtt_ssh &&ssh)
            : server_address{std::forward<Str>(server_address)},
              user{std::move(user)},
              ssh{std::move(ssh)}
        {
        }
        Traffic_mqtt_controller(Traffic_mqtt_controller &&) = default;
        Traffic_mqtt_controller &operator=(Traffic_mqtt_controller &&) = default;

        virtual void run() = 0;
    };
}