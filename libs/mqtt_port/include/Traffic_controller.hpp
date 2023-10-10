#pragma once

#include <Traffic_receiver.hpp>
#include <Traffic_sender.hpp>
#include <Traffic_channel_connector.hpp>

namespace Mqtt_port
{
    class Traffic_controller : public Traffic_receiver, public Traffic_sender, public Traffic_channel_connector
    {
    protected:
        Traffic_controller(const Traffic_controller &) = delete;
        Traffic_controller& operator=(const Traffic_controller &) = delete;

        virtual void write_exec(const std::string &channel_name, const Data &data, std::size_t write_len) = 0;

        /* Callbacks */
        virtual void write_usr_callback(std::size_t write_len) = 0;
        virtual void read_usr_callback(const Data &begin, std::size_t write_len) = 0;
        virtual void write_err_usr_callback(const int code) = 0;
        virtual void read_err_usr_callback(const int code) = 0;

    public:
        Traffic_controller() = default;
        Traffic_controller(Traffic_controller &&) = default;
        Traffic_controller& operator=(Traffic_controller &&) = default;
        virtual ~Traffic_controller() = 0;


        virtual void run() = 0;
        virtual void write(const std::string &channel_name, const Data &data, std::size_t write_len) override final;
    };
}