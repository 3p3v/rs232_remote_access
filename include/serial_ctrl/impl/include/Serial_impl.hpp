#pragma once

#include <Dyn_serial.hpp>
#include <Serial_info.hpp>
#include <Port_settings.hpp>
#include <Ctrl_interface.hpp>
#include <memory>

using namespace Phy_serial;
using namespace Serial_port;
using namespace Logic;

namespace Impl
{
    class Serial_impl final
    {
    private:
        /// @brief Information required to connect to serial port
        Serial_info &info;

        std::unique_ptr<Serial> serial{nullptr};

        template <typename Callb_t>
        friend class Serial_callb;

        /// @brief A way around for error C3848, which occurs while passing mutable lambda to lambda (for some reason???)
        /// @tparam Callb_t
        template <typename Callb_t>
        class Serial_callb
        {
            Callb_t ok_callb;
            Serial_impl &impl;

        public:
            template <typename Iter_t>
            void operator()(Iter_t begin, size_t len)
            {
                ok_callb(
                    begin,
                    begin + len,
                    [this](auto begin, auto end)
                    {
                        /* Resume reading */
                        impl.serial->listen(begin, end - begin);
                    });
            }

            Serial_callb(Callb_t &&ok_callb, Serial_impl &impl)
                : ok_callb{std::move(ok_callb)}, impl{impl}
            {
            }

            Serial_callb(const Callb_t &ok_callb, Serial_impl &impl)
                : ok_callb{ok_callb}, impl{impl}
            {
            }
        };

    public:
        Serial_impl(Serial_info &info);
        Serial_impl(Serial_impl &&) = default;
        Serial_impl &operator=(Serial_impl &&) = default;
        Serial_impl(const Serial_impl &) = delete;
        Serial_impl &operator=(const Serial_impl &) = delete;
        ~Serial_impl() = default;

        /********************************** Settings  **********************************/
    private:
        unsigned int baud_rate_trans(Port_settings::Baud_rate baud_rate);
        Ctrl_defs::Parity parity_trans(Port_settings::Parity parity);
        unsigned char char_size_trans(Port_settings::Char_size char_size);
        Ctrl_defs::Stop_bits stop_bits_trans(Port_settings::Stop_bits stop_bits);

    public:
        void set_baud_rate(Port_settings::Baud_rate baud_rate);
        void set_parity(Port_settings::Parity parity);
        void set_char_size(Port_settings::Char_size char_size);
        void set_stop_bits(Port_settings::Stop_bits stop_bits);

        /********************************** Data  **********************************/
    public:
        /// @brief Write to serial, use callbacks
        /// @tparam Iter_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param begin
        /// @param end
        /// @param ok_callb
        /// @param ec_callb
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        /// @brief Connect to serial
        /// @tparam Iter_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param begin
        /// @param end
        /// @param ok_callb
        /// @param ec_callb
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void connect(Iter_t begin,
                     Iter_t end,
                     Ok_callb &&ok_callb,
                     Ec_callb &&ec_callb);
    };

    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    inline void Serial_impl::write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        try
        {
            serial.write(
                begin,
                end,
                [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb = std::forward<Ec_callb>(ec_callb)](auto &ec, auto write_len)
                {
                if (ec == 0)
                {
                    ok_callb(write_len);
                }
                else
                {
                    ec_callb(ec);
                } });
        }
        catch (const boost::exception &e)
        {
        }
        {
            // throw Exception::Serial_except{e.what()};
        }
    }

    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    inline void Serial_impl::connect(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        if (serial == nullptr)
        {
            /* Create serial, set callbacks */
            serial = make_unique_dyn_serial(
                info.port,
                Serial_callb<std::remove_reference_t<decltype(ok_callb)>>{std::forward<Ok_callb>(ok_callb), *this},
                // [ok_callb = std::forward<Ok_callb>(ok_callb), this, ptr = shared_from_this()](auto data, auto read_len)
                // {
                //     ok_callb(data, data + read_len, [this](auto begin, auto end)
                //              {
                //                 /* Resume reading */
                //                 serial->listen(begin, end - begin); });
                // },
                [ec_callb = std::forward<Ec_callb>(ec_callb)](const unsigned int code, const std::string &err)
                {
                    // ec_callb(Exception::Serial_except{err});
                });

            try
            {
                /* Run serial */
                serial->run(begin, end - begin);
            }
            catch (const boost::exception &e)
            {
                // throw Exception::Serial_except{e.what()};
            }
        }
    }
}