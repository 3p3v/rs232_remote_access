#pragma once

#include <Traffic_controller.hpp>
#include <Traffic_mqtt_connect_options.hpp>
#include <Traffic_mqtt_user.hpp>
#include <Traffic_mqtt_ssh.hpp>
#include <string>
#include <set>

namespace Mqtt_port
{
    class Traffic_pub_callbacks
    {
    public:
        virtual void write_usr_callback(std::size_t write_len) = 0;
        virtual void write_err_usr_callback(const int code) = 0;
    };
    
    class Traffic_rec_callbacks
    {
    public:
        virtual void read_usr_callback(const std::vector<unsigned char> &begin, std::size_t write_len) = 0;
        virtual void read_err_usr_callback(const int code) = 0;
    };

    class Traffic_con_callbacks
    {
    public:
        virtual void connect_usr_callback(const std::string &cause) = 0;
        virtual void subscribre_usr_callback() = 0;
        virtual void connect_err_usr_callback(const std::string &cause) = 0;
        virtual void subscribre_err_usr_callback(const int code) = 0;
    };
    
    class Traffic_mqtt_callbacks : public Traffic_pub_callbacks,
                                   public Traffic_rec_callbacks, 
                                   public Traffic_con_callbacks
    {
        
        
        
    };

    class Traffic_mqtt_controller : public Traffic_controller
                                    // public virtual mqtt::callback,
					                // public virtual mqtt::iaction_listener
    {
        Traffic_mqtt_controller(Traffic_mqtt_controller &) = delete;
        Traffic_mqtt_controller &operator=(Traffic_mqtt_controller &) = delete;

    protected:
        std::string server_address;
        Traffic_mqtt_user user;
        Traffic_mqtt_ssh ssh;
        std::unique_ptr<Traffic_mqtt_callbacks> callbacks;

        virtual void connect() = 0;
        virtual void subscribre(const std::string &channel_name) = 0;
        // virtual void subscribre_exec(const string & channel_name) = 0;
        // virtual void connect_usr_callback(const std::string &cause) = 0;
        virtual void connect_callback(const std::string &cause) = 0;
        virtual void subscribre_callback() = 0;
        // virtual void subscribre_usr_callback() = 0;
        // virtual void connect_err_usr_callback(const std::string &cause) = 0;
        // virtual void subscribre_err_usr_callback(const int code) = 0;

    public:
        template <typename Str>
        Traffic_mqtt_controller(Str &&server_address,
                                Traffic_mqtt_user &&user,
                                Traffic_mqtt_ssh &&ssh,
                                Traffic_mqtt_callbacks &&callbacks)
            : server_address{std::forward<Str>(server_address)},
              user{std::move(user)},
              ssh{std::move(ssh)},
              callbacks{std::make_unique<Traffic_mqtt_callbacks>(std::move(callbacks))}
        {
        }
        Traffic_mqtt_controller(Traffic_mqtt_controller &&) = default;
        Traffic_mqtt_controller &operator=(Traffic_mqtt_controller &&) = default;

        virtual void run() = 0;
    };
}