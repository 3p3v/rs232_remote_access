#pragma once

#include <C_callb.hpp>

namespace Mqtt_port
{
    class Conn_callb : public C_callb
    {
    public:
        Conn_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector);

        void fail() override;

        void success() override;
    };
}