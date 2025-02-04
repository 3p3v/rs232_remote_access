#pragma once

namespace Logic
{
    /// @brief Base class for storeing exchanger module
    class Proto_module
    {
    public:
        virtual void restart() = 0;

        Proto_module() = default;
        Proto_module(Proto_module &&) = default;
        Proto_module(const Proto_module &) = default;
        Proto_module& operator=(Proto_module &&) = default;
        Proto_module& operator=(const Proto_module &) = default;
        virtual ~Proto_module() = default;
    };
}