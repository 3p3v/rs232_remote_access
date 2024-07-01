#pragma once

#include <memory>
#include <stdexcept>
#include <string_view>
#include <Mqtt_defs.hpp>
#include <Mqtt_except.hpp>
#include <Mqtt_fatal_except.hpp>
#include <Protocol_except.hpp>
#include <impl/Controller.hpp>
#include <Remote_info.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Wraps full remote functionality to few functions
    class Mqtt_impl final
    {
        /// @brief MQTT QOS
        static constexpr unsigned char qos{Mqtt_defs::qos};

        /// @brief
        static constexpr std::string_view packet_num_s{Topic_defs::packet_num_s};

    public:
        using Mqtt_controller = Mqtt_port::Impl::Controller;

    private:
        /// @brief MQTT client implementaton
        Mqtt_controller &controller;

        /// @brief Device MQTT related information
        std::shared_ptr<Remote_info> info;

        bool moved{false};

    public:
        Mqtt_impl(
            std::shared_ptr<Remote_info> &&info,
            Mqtt_controller &controller);
        Mqtt_impl(
            const std::shared_ptr<Remote_info> &info,
            Mqtt_controller &controller);
        Mqtt_impl(Mqtt_impl &&) noexcept;
        Mqtt_impl &operator=(Mqtt_impl &&) = delete;
        Mqtt_impl(const Mqtt_impl &) = delete;
        Mqtt_impl &operator=(const Mqtt_impl &) = delete;
        ~Mqtt_impl();

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
                info->info_ch,
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
                    ec_callb(Mqtt_except{"Exception was thrown while writing to info channel, code: " + std::to_string(code)});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Mqtt_fatal_except{e.what()};
            throw;
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
                info->info_ch,
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
                    ec_callb(Mqtt_except{"Exception was thrown while writing to info channel, code: " + std::to_string(code)});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Mqtt_fatal_except{e.what()};
            throw;
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
                info->set_ch,
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
                    ec_callb(Mqtt_except{"Exception was thrown while writing to set channel, code: " + std::to_string(code)});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Mqtt_fatal_except{e.what()};
            throw;
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
                info->set_ch,
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
                    ec_callb(Mqtt_except{"Exception was thrown while writing to set channel, code: " + std::to_string(code)});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Mqtt_fatal_except{e.what()};
            throw;
            
        }
    }

    template <typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::connect_i(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            controller.subscribe(
                info->info_ch,
                qos,
                /* Data receive callback */
                [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb](auto &&c)
                {
                    /* Run callback, pass message so it will not get deallocated */
                    auto begin = c->cbegin();
                    auto end = c->cend();
                    ok_callb(begin, end, [c = std::forward<decltype(c)>(c)]() {

                    });
                },
                /* Error callback */
                [ec_callb](int code)
                {
                    ec_callb(Mqtt_except{"Exception was thrown while reading from info channel, code: " + std::to_string(code)});
                },
                /* Subscribe success */
                []() {

                },
                /* Subscribe fail */
                [ec_callb](int code)
                {
                    ec_callb(Mqtt_except{"Exception was thrown while subscribing to info channel, code: " + std::to_string(code)});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Mqtt_fatal_except{e.what()};
            throw;
        }
    }

    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::write(char id, Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            controller.write(
                info->data_ch,
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
                    ec_callb(Mqtt_except{"Exception was thrown while writing to data channel, code: " + std::to_string(code)});
                });
        }
        catch (const mqtt::exception &e)
        {
            // throw Mqtt_fatal_except{e.what()};
            throw;
        }
    }

    template <typename Ok_callb, typename Ec_callb>
    inline void Mqtt_impl::connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            controller.subscribe(
                info->data_ch,
                qos,
                /* Data receive callback */
                [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb](auto &&c)
                {
                    // TODO move getting packet number to mqtt lib
                    /* Get packet number */
                    auto prop_begin = c->get_properties().c_struct().array;
                    auto prop_end = prop_begin + c->get_properties().c_struct().count;

                    if (prop_begin == prop_end)
                    {
                        /* Properties, show error to user */
                        ec_callb(Protocol_except{"No packet properties found! Protocol breach!"});
                    }

                    auto i_ = std::find_if(
                        prop_begin,
                        prop_end,
                        [&](auto &&prop)
                        {
                            /* Find user property */
                            if (prop.identifier == MQTTPROPERTY_CODE_USER_PROPERTY)
                            {
                                /* Find packet number */
                                if (prop.value.data.len == packet_num_s.size() && *prop.value.data.data == packet_num_s[0])
                                {
                                    auto p_num = prop.value.value.data[0];

                                    ///////////////////////// OK /////////////////////////
                                    /* Run callback, pass message so it will not get deallocated */
                                    ok_callb(p_num, c->cbegin(), c->cend(), [c = std::forward<decltype(c)>(c)]() {

                                    });
                                    //////////////////////////////////////////////////////

                                    return true;
                                }
                                else
                                {
                                    /* Packet number was not found, show error to user */
                                    ec_callb(Protocol_except{"No packet nuber was received! Protocol breach!"});

                                    return true;
                                }
                            }

                            return false;
                        });
                },
                /* Error callback */
                [ec_callb](int code)
                {
                    ec_callb(Mqtt_except{"Exception was thrown while reading from data channel, code: " + std::to_string(code)});
                },
                /* Subscribe success */
                []() {

                },
                /* Subscribe fail */
                [ec_callb](int code)
                {
                    ec_callb(Mqtt_except{"Exception was thrown while subscribing to data channel, code: " + std::to_string(code)});
                });
        }
        catch (const Mqtt_except &e)
        {
            // throw Mqtt_fatal_except{e.what()};
            throw;
        }
    }
}