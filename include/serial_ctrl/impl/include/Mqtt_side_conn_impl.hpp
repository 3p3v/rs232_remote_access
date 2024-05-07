#pragma once

#include <Mqtt_impl_base.hpp>

namespace Impl
{
    class Mqtt_side_conn_impl : public Mqtt_impl_base
    {
    public:
        /// @brief Read from device to buffer, use callbacks
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param ok_callb
        /// @param ec_callb
        template <typename Ok_callb, typename Ec_callb>
        void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Dev_info_ptr_t>
        Mqtt_side_conn_impl(Dev_info_ptr_t &&device, Mqtt_controller &controller);
    };

    template <typename Ok_callb, typename Ec_callb>
    inline void Mqtt_side_conn_impl::connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            controller.subscribe(
                device->get_data_ch(),
                qos,
                /* Data receive callback */
                [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb, this](auto &&c)
                {
                    /* Find user property */
                    if (c.identifier == MQTTPROPERTY_CODE_USER_PROPERTY)
                    {
                        /* Find packet number */
                        if (c.value.data.len == packet_num_s.size() && *c.value.data.data == packet_num_s[0])
                        {
                            /* Get number */
                            auto num = c.value.value.data[0];

                            /* Run callback, pass message so it will not get deallocated */
                            ok_callb(num, c->begin(), c->end(), [c = std::forward<decltype(c)>(c)]() {

                            });
                        }
                        else
                        {
                            /* Packet number was not found, show error to user */
                            ec_callb(Exception::Cmds_except{"No packet number, this should never happen!"});
                        }
                    }
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
            ec_callb(Exception::Mqtt_read_except{e.what()});
        }
    }

    template <typename Dev_info_ptr_t>
    inline Mqtt_side_conn_impl::Mqtt_side_conn_impl(Dev_info_ptr_t &&device, Mqtt_controller &controller)
        : Mqtt_impl_base{std::forward<Dev_info_ptr_t>(device), controller}
    {
    }
}