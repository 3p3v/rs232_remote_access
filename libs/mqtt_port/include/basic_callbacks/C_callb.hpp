#pragma once

#include <Callb.hpp>
#include <memory>
#include <Connector.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Controller;
    }

    class C_callb : public Callb
    {
    protected:
        std::shared_ptr<Connector> connector;

    public:
        C_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector);
        void fail() = 0;
        virtual void success() = 0;
    };
}