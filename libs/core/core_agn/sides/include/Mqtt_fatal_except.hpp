#pragma once

#include <Mqtt_except.hpp>

namespace Logic
{
    /// @brief Base extension exception class
    class Mqtt_fatal_except : public Mqtt_except
    {
        using Mqtt_except::Mqtt_except;
    };
}
// }