#pragma once

#include <iostream>
#include <exception>
#include <memory>
#include <boost/asio.hpp>
#include <Connection.hpp>
#include <Ctrl_interface.hpp>

namespace Serial_port
{
    class Serial
        : public Connection,
          public Ctrl_interface
    {
        bool moved{false};
        boost::asio::serial_port serial;
        std::string port;

    protected:
        void open();
        void close();

    public:
        template <typename Str>
        Serial(Io_context_ptr io_context_, Str &&port);
        Serial(Serial &&);
        Serial &operator=(Serial &&) = delete;
        Serial(Serial &) = delete;
        Serial &operator=(Serial &) = delete;
        virtual ~Serial();

        /* Conf */
        std::string get_port();
        void set_baud_rate(unsigned int baud_rate) override final;
        void set_parity(const Parity parity) override final;
        void set_char_size(unsigned int char_size) override final;
        void set_flow_ctrl(const Flow_ctrl flow_ctrf) override final;
        void set_stop_bits(const Stop_bits stop_bits) override final;
        void set_baud_rate() override final;
        void set_parity() override final;
        void set_char_size() override final;
        void set_flow_ctrl() override final;
        void set_stop_bits() override final;

        /* Communication */
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        /* Start */
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void run(Iter_t iter, size_t len, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void listen(Iter_t iter, size_t len, Ok_callb &&ok_callb, Ec_callb &&ec_callb);
    };

    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    void Serial::run(Iter_t iter, size_t len, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        open();
        listen<Iter_t>(iter, len, std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb));
    }

    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    void Serial::listen(Iter_t iter, size_t len, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        serial.async_read_some(
            boost::asio::buffer(&(*iter), len),
            [iter,
             ok_callb = std::forward<Ok_callb>(ok_callb),
             ec_callb = std::forward<Ec_callb>(ec_callb)](const boost::system::error_code &err, std::size_t read_len) mutable
            {
                if (err)
                {
                    ec_callb(err.value(), err.what());
                }
                else
                {
                    ok_callb(iter, read_len);
                }
            });
    }

    template <typename Str>
    Serial::Serial(Io_context_ptr io_context_, Str &&port)
        : Connection{io_context_}, serial{get_io_context()}, port{std::forward<Str>(port)}
    {
    }

    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    void Serial::write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        static constexpr auto size_ = sizeof(typename std::iterator_traits<Iter_t>::value_type);

        boost::asio::async_write(
            serial,
            boost::asio::buffer(&(*begin), (end - begin) * size_),
            [ok_callb = std::forward<Ok_callb>(ok_callb),
             ec_callb = std::forward<Ec_callb>(ec_callb)](const boost::system::error_code &err, std::size_t write_len)
            {
                if (err)
                    ec_callb(err.value(), err.what());
                else
                    ok_callb(write_len);
            });
    }
}