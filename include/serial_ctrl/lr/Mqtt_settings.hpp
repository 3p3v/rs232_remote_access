#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>
#include <Mqtt_except.hpp>
#include <Cmds_except.hpp>
#include <impl/Controller.hpp>
#include <Device.hpp>

class Mqtt_settings
{
    const Device &device;
    Mqtt_port::Impl::Controller &controller;
    static constexpr unsigned char qos{0};

public:
    /// @brief Write command with an argument
    /// @tparam Cont_t 
    /// @tparam Arg_cont_t 
    /// @tparam Ok_callb 
    /// @tparam Ec_callb 
    /// @param msg 
    /// @param arg 
    /// @param ok_callb 
    /// @param ec_callb 
    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    void write(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

    /// @brief Write command with no arguments
    /// @tparam Cont_t 
    /// @tparam Ok_callb 
    /// @tparam Ec_callb 
    /// @param msg 
    /// @param ok_callb 
    /// @param ec_callb 
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    void Mqtt_settings::write(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);


    /// @brief Connect and set callbacks
    /// @tparam Ok_callb
    /// @tparam Ec_callb
    /// @param device
    /// @param controller
    /// @param ok_callb
    /// @param ec_callb
    template <typename Ok_callb, typename Ec_callb>
    Mqtt_settings(const Device &device,
                  Mqtt_port::Impl::Controller &controller,
                  Ok_callb &&ok_callb,
                  Ec_callb &&ec_callb);
};

template <typename Cont_t, typename Ok_callb, typename Ec_callb>
inline void Mqtt_settings::write(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
{
    auto msg_ptr = std::make_unique<std::string>(std::string{msg} + "\n");

    auto beg = msg_ptr->cbegin();
    auto end = msg_ptr->cend();

    try
    {
        controller.write(
            device->get_info_ch(),
            qos,
            beg,
            end,
            [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
             msg = std::string{msg},
             ok_callb = std::forward<Callb>(ok_callb)
             this](size_t size)
            {
                ok_callb(size);
            };
            [ec_callb = std::forward<Ec_callb>(ec_callb)](int)
            {
                ec_callb(code);
            });
    }
    catch (const mqtt::exception &e)
    {
        throw Exception::Mqtt_read_except{e.what()};
    }
}

template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
inline void Mqtt_settings::write(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
{
    auto msg_ptr = std::make_unique<std::string>(std::string{msg} + " " + std::string{arg} + "\n");

    auto beg = msg_ptr->cbegin();
    auto end = msg_ptr->cend();

    try
    {
        controller.write(
            device->get_info_ch(),
            qos,
            beg,
            end,
            [ok_callb = std::forward<Ok_callb>(ok_callb),
             msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
             msg = std::string{msg},
             this](size_t size)
            {
                ok_callb(size);
            };
            [ec_callb = std::forward<Ec_callb>(ec_callb)](int code)
            {
                ec_callb(code);
            });
    }
    catch (const mqtt::exception &e)
    {
        throw Exception::Mqtt_read_except{e.what()};
    }
}

template <typename Ok_callb, typename Ec_callb>
inline Mqtt_settings::Mqtt_settings(const Device &device, Mqtt_port::Impl::Controller &controller, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    : device{device}, controller{controller}
{
    try
    {
        controller.subscribe(
            device->info_ch(),
            qos,
            /* Data receive callback */
            [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb, this](auto &&c)
            {
                /* Run callback, pass message so it will not get deallocated */
                ok_callb(num, c->begin(), c->end(), [c = std::forward<decltype(c)>(c)]() {

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