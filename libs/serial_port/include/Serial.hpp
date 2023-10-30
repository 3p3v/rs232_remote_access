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
        Serial(Serial &) = delete;
        Serial &operator=(Serial &) = delete;

        boost::asio::serial_port serial;
        std::string port;

        template <typename Cont>
        void write(const Cont &data, const size_t data_len);

    protected:
        void open();
        void close();

    public:
        template <typename Str>
        Serial(Io_context_ptr io_context_, Str &&port);
        Serial(Serial &&) = default;
        Serial &operator=(Serial &&) = default;
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
        void write(const std::vector<unsigned char> &data, const size_t data_len);
        void write(const std::string &data, const size_t data_len);
        template <typename Cont>
        void write(const typename Cont::const_iterator begin, const typename Cont::const_iterator end); // override final;
        
        /* Start */
        void run() override final;
    };

    template <typename Str>
    Serial::Serial(Io_context_ptr io_context_, Str &&port)
        : Connection{io_context_}, serial{get_io_context()}, port{std::forward<Str>(port)}
    {
    }

    template <typename Cont>
    void Serial::write(const Cont &data, const size_t data_len)
    {
        boost::asio::async_write(serial,
                                 boost::asio::buffer(data, data_len),
                                 [this](const boost::system::error_code &err, std::size_t write_len)
                                 {
                                     if (err)
                                         error_callback(err.value(), err.what());
                                     else
                                         write_callback(write_len);
                                 });
    }

    template <typename Cont>
    void Serial::write(const typename Cont::const_iterator begin, const typename Cont::const_iterator end)
    {
        static constexpr size_ = sizeof(typename std::iterator_traits<typename Cont::const_iterator>::value_type);
        
        boost::asio::async_write(serial,
                                 boost::asio::buffer(&(*begin), (end - begin) * size_),
                                 [this](const boost::system::error_code &err, std::size_t write_len)
                                 {
                                     if (err)
                                         error_callback(err.value(), err.what());
                                     else
                                         write_callback(write_len);
                                 });
    }
}