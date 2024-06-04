#pragma once

#include <Callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Controller;
    }

    class O_callb : public Callb
    {
    public:
        O_callb(Impl::Controller &controller);
        void fail() = 0;
        virtual void success(const std::string &channel_name, size_t len) = 0;
    };
}