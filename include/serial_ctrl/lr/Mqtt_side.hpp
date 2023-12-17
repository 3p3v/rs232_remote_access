#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>
#include <Mqtt_except.hpp>
#include <Cmds_except.hpp>
#include <impl/Controller.hpp>
#include <Device.hpp>

class Mqtt_side
{
    const Device &device;
    // std::atomic_bool inited{false};
    Mqtt_port::Impl::Controller &controller;
    static constexpr unsigned char qos{0};
    static constexpr std::string_view packet_num_s{"P"};

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
    void write(unsigned char id, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

    // /// @brief Read from device to buffer, use callbacks
    // /// @tparam Ok_callb
    // /// @tparam Ec_callb
    // /// @param ok_callb
    // /// @param ec_callb
    // template <typename Ok_callb, typename Ec_callb>
    // void read(Ok_callb &&ok_callb, Ec_callb &&ec_callb) &&;

    /// @brief Connect and set callbacks
    /// @tparam Ok_callb 
    /// @tparam Ec_callb 
    /// @param device 
    /// @param controller 
    /// @param ok_callb 
    /// @param ec_callb 
    template <typename Ok_callb, typename Ec_callb>
    Mqtt_side(const Device &device,
              Mqtt_port::Impl::Controller &controller,
              Ok_callb &&ok_callb,
              Ec_callb &&ec_callb);
};

template <typename Cont_t, typename Ok_callb, typename Ec_callb>
inline void Mqtt_side::write(unsigned char id, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
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

template <typename Ok_callb, typename Ec_callb>
inline Mqtt_side::Mqtt_side(const Device &device, Mqtt_port::Impl::Controller &controller, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    : device{device}, controller{controller}
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

// template <typename Ok_callb, typename Ec_callb>
// void Mqtt_side::read(Ok_callb &&ok_callb, Ec_callb &&ec_callb) &&
// {
//     try
//     {
//         controller.subscribe(
//             device->get_data_ch(),
//             qos,
//             /* Data receive callback */
//             [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb, this](auto &&c)
//             {
//                 /* Find user property */
//                 if (c.identifier == MQTTPROPERTY_CODE_USER_PROPERTY)
//                 {
//                     /* Find packet number */
//                     if (c.value.data.len == packet_num_s.size() && *c.value.data.data == packet_num_s[0])
//                     {
//                         /* Get number */
//                         auto num = c.value.value.data[0];

//                         /* Run callback, pass message so it will not get deallocated */
//                         ok_callb(num, c->begin(), c->end(), [c = std::forward<decltype(c)>(c)]() {

//                         });
//                     }
//                     else
//                     {
//                         /* Packet number was not found, show error to user */
//                         ec_callb(Exception::Cmds_except{"No packet number, this should never happen!"});
//                     }
//                 }
//             },
//             /* Error callback */
//             [ec_callb](int)
//             {
//                 ec_callb(Exception::Mqtt_read_except{});
//             },
//             /* Subscribe success */
//             []() {

//             },
//             /* Subscribe fail */
//             [ec_callb](int)
//             {
//                 ec_callb(Exception::Mqtt_read_except{});
//             });
//     }
//     catch (const std::exception &e)
//     {
//         ec_callb(Exception::Mqtt_read_except{e.what()});
//     }
// }