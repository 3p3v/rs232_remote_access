#pragma once

#include <Dyn_serial.hpp>
#include <Serial_info.hpp>
#include <Port_settings.hpp>
#include <Ctrl_interface.hpp>
#include <memory>
#include <Serial_except.hpp>
#include <Serial_fatal_except.hpp>
#include <boost/exception/diagnostic_information.hpp>

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

        Dyn_serial serial;

        template <typename Callb_t>
        friend class Serial_callb;

        /// @brief A way around for error C3848, which occurs while passing mutable lambda to lambda (for some reason???)
        /// @tparam Callb_t
        template <typename Callb_t, typename Ec_callb_t>
        class Serial_callb
        {
            template <typename Callb_t_, typename Ec_callb_t_>
            friend class Post_callb;
            
            /// @brief Runde after user callback to continue reading from port
            class Post_callb
            {
                Ec_callb_t ec_callb_;
                Serial_callb<Callb_t, Ec_callb_t> &serial_callb;

            public: 
                template <typename Iter_t>
                inline void operator()(Iter_t begin, Iter_t end) const
                {
                    /* Resume reading */
                    serial_callb.impl.serial.listen(begin, end - begin, std::move(serial_callb), std::move(ec_callb_));
                }  

                template <typename Ec_callb_t_t>
                Post_callb(Ec_callb_t_t &&ec_callb_, Serial_callb<Callb_t, Ec_callb_t> &serial_callb)
                    : ec_callb_{std::forward<Ec_callb_t_t>(ec_callb_)}, serial_callb{serial_callb}
                {
                }
            };
            
            Callb_t ok_callb;
            Ec_callb_t ec_callb;
            Serial_impl &impl;

        public:
            template <typename Iter_t>
            void operator()(Iter_t begin, size_t len)
            {
                ok_callb(
                    begin,
                    begin + len,
                    // Next callbac will only run if device still exists
                    Post_callb(std::move(ec_callb), *this));
            }

            template <typename Callb_t_t, typename Ec_callb_t_t>
            inline Serial_callb(Callb_t_t &&ok_callb, Ec_callb_t_t &&ec_callb, Serial_impl &impl)
                : ok_callb{std::forward<Callb_t_t>(ok_callb)}, ec_callb{std::forward<Ec_callb_t_t>(ec_callb)}, impl{impl}
            {
            }
        };

    public:
        Serial_impl(Serial_info &info);
        inline Serial_impl(Serial_impl &&si) noexcept
            : info{si.info}, serial{std::move(si.serial)}
        {
        }
        Serial_impl &operator=(Serial_impl &&) = delete;
        Serial_impl(const Serial_impl &) = delete;
        Serial_impl &operator=(const Serial_impl &) = delete;
        inline ~Serial_impl()
        {
        }

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
                [ok_callb = std::forward<Ok_callb>(ok_callb)](auto write_len)
                {
                    ok_callb(write_len);
                },
                [ec_callb = std::forward<Ec_callb>(ec_callb)](int code, auto &&message)
                {
                    ec_callb(Serial_except{std::forward<decltype(message)>(message), code});
                });
        }
        catch (const boost::exception &e)
        {
            // throw Serial_fatal_except{boost::diagnostic_information(e)};
            throw;
        }
    }

    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    inline void Serial_impl::connect(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        /* Set callbacks */
        auto ec = [ec_callb = std::forward<Ec_callb>(ec_callb)](int code, auto &&message)
        {
            ec_callb(Serial_except{std::forward<decltype(message)>(message), code});
        };
        
        auto full = Serial_callb<
            std::remove_reference_t<Ok_callb>,
            std::remove_reference_t<decltype(ec)>>{std::forward<Ok_callb>(ok_callb), ec, *this};

        try
        {
            /* Run serial */
            serial.run(begin, end - begin, std::move(full), std::move(ec));
        }
        catch (const boost::exception &e)
        {
            // throw Serial_fatal_except{boost::diagnostic_information(e)};
            throw;
        }
    }
}