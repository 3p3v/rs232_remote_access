#pragma once

#include <Serial.hpp>
#include <mutex>

namespace Phy_serial
{
    class Serial_context
    {
    protected:
        /* Io_context used by every serial */
        static std::shared_ptr<boost::asio::io_context> shared_io_context_;
        static std::atomic_bool is_running;

    public:
        Serial_context() = default;
        Serial_context(Serial_context&&) = default;
        Serial_context& operator=(Serial_context&&) = default;
        Serial_context(Serial_context&) = default;
        Serial_context& operator=(Serial_context&) = default;
        virtual ~Serial_context() = 0;

        static void run();

        
        // static std::mutex run_guard;
    };
}
