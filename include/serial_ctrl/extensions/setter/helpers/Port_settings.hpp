#pragma once

namespace Logic
{
    namespace Port_settings
    {
        enum class Parity
        {
            even = 0,
            none,
            odd
        };

        enum class Stop_bits
        {
            one = 0,
            onepointfive,
            two
        };

        using Baud_rate = unsigned int;

        using Char_size = unsigned int;
    };

    
    
}