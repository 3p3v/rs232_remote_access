#pragma once

#include <Conn_callb_impl.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        Conn_callb_impl::Conn_callb_impl(Grip &conn_callback)
            : Callb_impl<C_callb>{conn_callback}
        {
        }

        void Conn_callb_impl::on_failure(const mqtt::token &asyncActionToken)
        {
            get_callb().fail();
        }

        void Conn_callb_impl::on_success(const mqtt::token &asyncActionToken)
        {
        }
    }
}