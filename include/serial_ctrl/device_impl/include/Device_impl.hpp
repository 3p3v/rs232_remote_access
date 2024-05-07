#pragma once

/* Protocol implementation */
#include <Device.hpp>
/* Low-level driver classes */
#include <Custom_timer.hpp>
#include <Mqtt_sett_conn_impl.hpp>
#include <Mqtt_side_conn_impl.hpp>
#include <Mqtt_sett_impl.hpp>
#include <Mqtt_side_impl.hpp>
#include <Serial_side_conn_impl.hpp>
#include <Serial_sett_impl.hpp>
#include <Serial_side_impl.hpp>

namespace Impl
{
    class Device_impl 
        : Device<
            Custom_timer,
            Mqtt_sett_conn_impl
            Mqtt_side_conn_impl
            Mqtt_sett_impl
            Mqtt_side_impl
            Serial_side_conn_impl
            Serial_sett_impl
            Serial_side_impl>
    {
    public:
        Device_impl();
    };
}