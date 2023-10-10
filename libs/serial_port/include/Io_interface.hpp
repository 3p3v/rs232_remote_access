#pragma once

#include <iostream>
#include <vector>
#include <exception>
#include <Connection_interface.hpp>
#include <Data_interface.hpp>

namespace Serial_port
{
    class Io_interface : public Connection_interface, public Data_interface
    {
    protected:
        Data buffer;
        
        /* Callbacks */
        virtual void write_callback(std::size_t write_len) = 0;
        virtual void read_callback(const Data &data, std::size_t write_len) = 0;
        virtual void error_callback(const unsigned int code, const std::string &err) = 0;
    };
}