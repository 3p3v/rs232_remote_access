#pragma once

#include <Serial.hpp>
#include <mutex>

namespace Phy_serial
{
    class Serial_context
    {
    protected:
        static std::atomic_bool is_running;

    public:
        Serial_context() = delete;

        /* Io_context used by every serial */
        static std::shared_ptr<boost::asio::io_context> shared_io_context_;
        
        static void run();
    };
}
