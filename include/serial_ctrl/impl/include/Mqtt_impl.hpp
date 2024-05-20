#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>
// #include <Mqtt_except.hpp>
// #include <Cmds_except.hpp>
#include <impl/Controller.hpp>
#include <Remote_info.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Wraps full remote functionality to few fenctions
    class Mqtt_impl final
    {
        /// @brief MQTT QOS
        static constexpr unsigned char qos{Topic_defs::qos};

        /// @brief 
        static constexpr std::string_view packet_num_s{Topic_defs::packet_num_s};

    public:
        using Mqtt_controller = Mqtt_port::Impl::Controller;

    private:
        /// @brief Device MQTT related information
        Remote_info &info;

        /// @brief MQTT client implementaton
        Mqtt_controller &controller;

    public:
        Mqtt_impl(
            Remote_info &device,
            Mqtt_controller &controller);
        Mqtt_impl(Mqtt_impl&&) = default;
        Mqtt_impl& operator=(Mqtt_impl&&) = default;
        Mqtt_impl(const Mqtt_impl&) = delete;
        Mqtt_impl& operator=(const Mqtt_impl&) = delete;
        ~Mqtt_impl() = default;
        
        /********************************** Settings  **********************************/
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

        /// @brief Connect to info channel, set callbacks
        /// @tparam Ok_callb 
        /// @tparam Ec_callb 
        /// @param ok_callb 
        /// @param ec_callb 
        template <typename Ok_callb, typename Ec_callb>
        void connect_i(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        /********************************** Data  **********************************/
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
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void write(char id, Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        /// @brief Read from device to buffer, use callbacks
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param ok_callb
        /// @param ec_callb
        template <typename Ok_callb, typename Ec_callb>
        void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb);
        
    };

    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        try
        {
            controller.write(
                info.info_ch,
                qos,
                beg,
                end,
                [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                 msg = std::string{msg},
                 ok_callb = std::forward<Ok_callb>(ok_callb), this](size_t size)
                {
                    ok_callb();
                },
                [ec_callb = std::forward<Ec_callb>(ec_callb)](int code)
                {
                    ec_callb(std::exception{""});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Exception::Mqtt_read_except{e.what()};
        }
    }

    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + " " + std::string{arg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        try
        {
            controller.write(
                info.info_ch,
                qos,
                beg,
                end,
                [ok_callb = std::forward<Ok_callb>(ok_callb),
                 msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                 msg = std::string{msg},
                 this](size_t size)
                {
                    ok_callb();
                },
                [ec_callb = std::forward<Ec_callb>(ec_callb)](int code)
                {
                    ec_callb(std::exception{""});
                });
        }
        catch (const mqtt::exception &e)
        {
            throw Exception::Mqtt_read_except{e.what()};
        }
    }

    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        try
        {
            controller.write(
                info.set_ch,
                qos,
                beg,
                end,
                [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                 msg = std::string{msg},
                 ok_callb = std::forward<Ok_callb>(ok_callb), this](size_t size)
                {
                    ok_callb();
                },
                [ec_callb = std::forward<Ec_callb>(ec_callb)](int code)
                {
                    ec_callb(std::exception{""});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Exception::Mqtt_read_except{e.what()};
        }
    }

    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + " " + std::string{arg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        try
        {
            controller.write(
                info.set_ch,
                qos,
                beg,
                end,
                [ok_callb = std::forward<Ok_callb>(ok_callb),
                 msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                 msg = std::string{msg},
                 this](size_t size)
                {
                    ok_callb();
                },
                [ec_callb = std::forward<Ec_callb>(ec_callb)](int code)
                {
                    ec_callb(std::exception{""});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Exception::Mqtt_read_except{e.what()};
        }
    }

    template <typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::connect_i(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            controller.subscribe(
                info.info_ch,
                qos,
                /* Data receive callback */
                [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb](auto &&c)
                {
                    /* Run callback, pass message so it will not get deallocated */
                    ok_callb(c->cbegin(), c->cend(), [c = std::forward<decltype(c)>(c)]() {

                    });
                },
                /* Error callback */
                [ec_callb](int)
                {
                    // ec_callb(Exception::Mqtt_read_except{});
                },
                /* Subscribe success */
                []() {

                },
                /* Subscribe fail */
                [ec_callb](int)
                {
                    // ec_callb(Exception::Mqtt_read_except{});
                });
        }
        catch (const std::exception &e)
        {
            // throw Exception::Mqtt_read_except{e.what()};
        }
    }

    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::write(char id, Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            controller.write(
                info.data_ch,
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
                    // ec_callb(Exception::Mqtt_read_except{code});
                });
        }
        catch (const std::exception &e)
        {
            // ec_callb(Exception::Mqtt_read_except{e.what()});
        }
    }
}