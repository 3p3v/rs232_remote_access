#pragma once

#include <Channel_conn_callb_impl.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        Channel_conn_callb_impl::Channel_conn_callb_impl(Grip &channel_conn_callback)
            : Callb_impl<C_callb>{channel_conn_callback}
        {
        }

        void Channel_conn_callb_impl::on_failure(const mqtt::token &asyncActionToken)
        {
            get_callb().fail();
        }

        void Channel_conn_callb_impl::on_success(const mqtt::token &asyncActionToken)
        {
            get_callb().success();
        }
    }
}