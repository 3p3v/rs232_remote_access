#pragma once

#include <Send_callb_impl.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        Send_callb_impl::Send_callb_impl(Grip &io_callb)
            : Callb_impl<O_callb>{io_callb}
        {
        }

        void Send_callb_impl::on_failure(const mqtt::token &asyncActionToken)
        {
            get_callb().fail();
        }

        void Send_callb_impl::on_success(const mqtt::token &asyncActionToken)
        {
        }
    }
}