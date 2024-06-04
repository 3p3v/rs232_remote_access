#pragma once

#include <Intf_except.hpp>

namespace Logic
{
    /// @brief Mqtt exception class
    class Mqtt_except : public Intf_except
    {
        using Intf_except::Intf_except;
    };
}