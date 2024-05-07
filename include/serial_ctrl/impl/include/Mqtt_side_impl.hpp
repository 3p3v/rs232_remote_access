#pragma once

#include <Mqtt_impl_base.hpp>

namespace Impl
{
    class Mqt_side_impl : public Mqtt_impl_base
    {
    public:
        /// @brief Write to device, use callbacks
        /// @tparam Cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param id
        /// @param begin
        /// @param end
        /// @param ok_callb
        /// @param ec_callb
        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        void write(char id, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Dev_info_ptr_t>
        Mqt_side_impl(Dev_info_ptr_t &&device, Mqtt_controller &controller);
    };

    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqt_side_impl::write(char id, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            controller.write(
                device->get_data_ch(),
                qos,
                std::string{packet_num_s},
                std::to_string(id),
                begin,
                end,
                [ok_callb = std::forward<Ok_callb>(ok_callb), this, id](size_t size)
                {
                    ok_callb(id, size);
                },
                [ec_callb](int code)
                {
                    ec_callb(Exception::Mqtt_read_except{code});
                });
        }
        catch (const std::exception &e)
        {
            ec_callb(Exception::Mqtt_read_except{e.what()});
        }
    }

    template <typename Dev_info_ptr_t>
    inline Mqt_side_impl::Mqt_side_impl(Dev_info_ptr_t &&device, Mqtt_controller &controller)
        : Mqtt_impl_base{std::forward<Dev_info_ptr_t>(device), controller}
    {
    }
}