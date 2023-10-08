#pragma once

#include <Traffic_receiver.hpp>
#include <Traffic_sender.hpp>

namespace Mqtt_port
{
    class Traffic_controller : public Traffic_receiver, public Traffic_sender
    {
    protected:
        virtual void write_exec(const std::string &channel_name, const Data_type &data) = 0;

        /* Callbacks */
        virtual void write_callback(std::size_t write_len) = 0;
        virtual void read_callback(Cont_type::iterator begin, Cont_type::iterator end, std::size_t write_len) = 0;
        virtual void error_callback(const unsigned int code, const unsigned int message_id) = 0;
    
    public:
        virtual void run() = 0;
        virtual void write(const std::string &channel_name, const Data_type &data) override final;
    };
}