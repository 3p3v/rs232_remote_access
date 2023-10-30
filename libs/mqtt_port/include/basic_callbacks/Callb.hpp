#pragma once

#include <Callbacks_defs.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Controller;
    }

    class Callb : protected Callbacks_defs
    {
    protected:
        Impl::Controller &controller;

    public:
        Callb(Impl::Controller &controller);
        Callb(Callb &&) = default;
        Callb &operator=(Callb &&) = default;
        Callb(Callb &) = delete;
        Callb &operator=(Callb &) = delete;
        ~Callb() = default;
        virtual void fail() = 0;
    };
}