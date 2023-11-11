#pragma once

#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include <Connection.hpp>
#include <Io_interface.hpp>
#include <Ctrl_interface.hpp>

namespace Serial_port
{
    class Serial : public Connection, public Io_interface, public Ctrl_interface
    {
        boost::asio::serial_port serial;
        std::string port;

    protected:
        void open();
        void close();

    public:
        template <typename Str>
        Serial(Io_context_ptr io_context_, Str &&port);
        Serial(Serial &&) = default;
        Serial &operator=(Serial &&) = default;
        Serial(Serial &) = delete;
        Serial &operator=(Serial &) = delete;
        virtual ~Serial() = 0;

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
        template <typename Cont, typename Ok_callb>
        void write(const typename Cont::const_iterator begin, const typename Cont::const_iterator end, Ok_callb &&ok_callb);

        /* Start */
        void run() override final;

        void listen();
    };

    template <typename Str>
    Serial::Serial(Io_context_ptr io_context_, Str &&port)
        : Connection{io_context_}, serial{get_io_context()}, port{std::forward<Str>(port)}
    {
    }

    template <typename Cont, typename Ok_callb>
    void Serial::write(const typename Cont::const_iterator begin, const typename Cont::const_iterator end, Ok_callb &&ok_callb)
    {
        static constexpr auto size_ = sizeof(typename std::iterator_traits<typename Cont::const_iterator>::value_type);

        boost::asio::async_write(serial,
                                 boost::asio::buffer(&(*begin), (end - begin) * size_),
                                 [this, ok_callb = std::forward<Ok_callb>(ok_callb)](const boost::system::error_code &err, std::size_t write_len)
                                 {
                                     if (err)
                                         error_callback(err.value(), err.what());
                                     else
                                         ok_callb(write_len);
                                 });
    }
}