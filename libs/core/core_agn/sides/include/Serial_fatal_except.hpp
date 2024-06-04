#pragma once

#include <Serial_except.hpp>

namespace Logic
{
    /// @brief Base extension exception class
    class Serial_fatal_except : public Serial_except
    {
        using Serial_except::Serial_except;
    };
}
// }