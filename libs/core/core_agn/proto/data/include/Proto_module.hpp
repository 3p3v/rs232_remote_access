#pragma once

#include <Proto_commons.hpp>

namespace Logic
{
    /// @brief Base class for storeing exchanger module
    class Base_proto_module
    {
    public:
        virtual void restart() = 0;

        Base_proto_module() = default;
        Base_proto_module(Base_proto_module &&) = default;
        Base_proto_module(const Base_proto_module &) = default;
        Base_proto_module& operator=(Base_proto_module &&) = default;
        Base_proto_module& operator=(const Base_proto_module &) = default;
        virtual ~Base_proto_module() = default;
    };

    template <typename Device_t>
    class Proto_module : public Base_proto_module, public Proto_commons<Device_t>
    {
        using Proto_commons<Device_t>::Proto_commons;
    };
}