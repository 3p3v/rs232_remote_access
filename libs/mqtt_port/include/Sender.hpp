#pragma once

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <Validator.hpp>
#include <Sender_intf.hpp>
#include <Def.hpp>

namespace Mqtt_port
{
    class Sender //: public Sender_intf
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

    
}