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
        std::vector<Data_type> buffer;
        
        /* Callbacks */
        virtual void write_callback(std::size_t write_len) = 0;
        virtual void read_callback(Cont_type::iterator begin, Cont_type::iterator end, std::size_t write_len) = 0;
        virtual void error_callback(const std::string &err) = 0;
    };
}