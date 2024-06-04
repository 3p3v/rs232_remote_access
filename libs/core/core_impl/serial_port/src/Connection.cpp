#pragma once

#include <iostream>
#include <memory>
#include <Connection.hpp>

namespace Serial_port
{
    Connection::Io_context &Connection::get_io_context() { return *io_context_; };

    Connection::Connection(Io_context_ptr io_context_)
        : io_context_{std::move(io_context_)}
    {
    }

    Connection::~Connection() 
    {
    }
}