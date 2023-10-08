#pragma once

#include <iostream>
#include <memory>
#include <boost/asio.hpp>

namespace Serial_port
{
    class Connection
    {
    public:
        using Io_context = boost::asio::io_context;
        using Io_context_ptr = const std::shared_ptr<boost::asio::io_context>;
        using Ptr_base = std::shared_ptr<Connection>;

    private:
        Io_context_ptr io_context_;

    protected:
        Io_context& get_io_context();

    public:
        Connection(Io_context_ptr io_context_);
        virtual ~Connection() = 0;

        virtual void run() = 0;
    };
}