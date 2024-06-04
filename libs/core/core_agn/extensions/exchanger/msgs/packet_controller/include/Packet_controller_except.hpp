#pragma once

#include <stdexcept>

namespace Logic
{
    class Packet_controller_except : public std::logic_error
    {
        using std::logic_error::logic_error;
    };
} // namespace Logic
