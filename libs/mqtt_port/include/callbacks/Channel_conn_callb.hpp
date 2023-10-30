#pragma once

#include <C_callb.hpp>

namespace Mqtt_port
{
    class Channel_conn_callb final : public C_callb
    {
    public:
        Channel_conn_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector);

        void fail() override;

        void success() override;
    };
}