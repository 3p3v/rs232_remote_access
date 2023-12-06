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
        /* Callbacks */
        virtual void read_callback(Data::iterator datadata, std::size_t write_len) = 0;
        virtual void error_callback(const unsigned int code, const std::string &err) = 0;
    };
}