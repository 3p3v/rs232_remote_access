#include <Serial_context.hpp>
#include <stdexcept>

namespace Phy_serial
{
    std::shared_ptr<boost::asio::io_context> Serial_context::shared_io_context_{new boost::asio::io_context{}};
    std::atomic_bool Serial_context::is_running{false};

    // Serial_context::~Serial_context() {}

    void Serial_context::run()
    {
        if (!is_running)
        {
            boost::system::error_code ec;
            
            shared_io_context_->run(ec);

            if (ec)
            {
                throw std::runtime_error{ec.what()};
            }
        }
    }
}