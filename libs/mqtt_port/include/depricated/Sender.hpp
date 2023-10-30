#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <Validator.hpp>
#include <Sender_intf.hpp>
#include <Def.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    class Sender //: public Validator
    {
        // using Data = std::vector<unsigned char>;
        
        std::shared_ptr<Validator> validator;

    protected:
        virtual void write_handle(const std::string &channel_name, const Data &data, std::size_t write_len) = 0;
        // virtual void write_exec(const std::string &channel_name, const Data &data, std::size_t write_len) = 0;
        

    public:
        bool write(const std::string &channel_name, const Data &data, std::size_t write_len);
        // void callback()
    
        Sender(std::shared_ptr<Validator> validator);
    };

    // class Paho_sender final : public Sender
    // {
    //     std::shared_ptr<mqtt::async_client> client;
    //     std::unique_ptr<mqtt::iaction_listener> callb;

    // protected:
    //     void write_handle(const std::string &channel_name, const Data &data, std::size_t write_len);

    // public:
    //     Paho_sender(std::shared_ptr<mqtt::async_client> client,
    //                 std::shared_ptr<Validator> &&validator,
    //                 std::unique_ptr<mqtt::iaction_listener> &&callb);
    // };
}