#pragma once

#include <Mqtt_impl_base.hpp>

using namespace Logic;

namespace Impl
{
    class Mqtt_sett_conn_impl : public Mqtt_impl_base
    {
    public:
        template <typename Ok_callb, typename Ec_callb>
        void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Dev_info_ptr_t>
        Mqtt_sett_conn_impl(
            Dev_info_ptr_t &&device,
            Mqtt_controller &controller);
    };

    template <typename Ok_callb, typename Ec_callb>
    inline void Mqtt_sett_conn_impl::connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            controller.subscribe(
                device->info_ch(),
                qos,
                /* Data receive callback */
                [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb](auto &&c)
                {
                    /* Run callback, pass message so it will not get deallocated */
                    ok_callb(c->begin(), c->end(), [c = std::forward<decltype(c)>(c)]() {

                    });
                },
                /* Error callback */
                [ec_callb](int)
                {
                    ec_callb(Exception::Mqtt_read_except{});
                },
                /* Subscribe success */
                []() {

                },
                /* Subscribe fail */
                [ec_callb](int)
                {
                    ec_callb(Exception::Mqtt_read_except{});
                });
        }
        catch (const std::exception &e)
        {
            throw Exception::Mqtt_read_except{e.what()};
        }
    }

    template <typename Dev_info_ptr_t>
    inline Mqtt_sett_conn_impl::Mqtt_sett_conn_impl(Dev_info_ptr_t &&device, Mqtt_controller &controller)
        : Mqtt_impl_base{std::forward<Dev_info_ptr_t>(device), controller}
    {
    }
}