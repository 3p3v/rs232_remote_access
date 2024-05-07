#pragma once

#include <Mqtt_impl_base.hpp>

using namespace Logic;

namespace Impl
{
    class Mqtt_sett_impl : public Mqtt_impl_base
    {
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
        void write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        /// @brief Write command with no arguments
        /// @tparam Cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param msg
        /// @param ok_callb
        /// @param ec_callb
        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        void write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

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
        void write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        /// @brief Write command with no arguments
        /// @tparam Cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param msg
        /// @param ok_callb
        /// @param ec_callb
        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        void write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Dev_info_ptr_t>
        Mqtt_sett_impl(
            Dev_info_ptr_t &&device,
            Mqtt_controller &controller);
    };

    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_sett_impl::write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        try
        {
            controller.write(
                info->info_ch(),
                qos,
                beg,
                end,
                [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                 msg = std::string{msg},
                 ok_callb = std::forward<Callb>(ok_callb) this](size_t size)
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
    inline void Mqtt_sett_impl::write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + " " + std::string{arg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        try
        {
            controller.write(
                info->info_ch(),
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

    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_sett_impl::write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        try
        {
            controller.write(
                info->set_ch(),
                qos,
                beg,
                end,
                [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                 msg = std::string{msg},
                 ok_callb = std::forward<Callb>(ok_callb) this](size_t size)
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
    inline void Mqtt_sett_impl::write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + " " + std::string{arg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        try
        {
            controller.write(
                info->set_ch(),
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

    template <typename Dev_info_ptr_t>
    inline Mqtt_sett_impl::Mqtt_sett_impl(Dev_info_ptr_t &&device, Mqtt_controller &controller)
        : Mqtt_impl_base{std::forward<Dev_info_ptr_t>(device), controller}
    {
    }
}