#pragma once

#include <iostream>
#include <exception>
#include <Serial.hpp>

namespace Serial_port
{
    Serial::Serial(Io_context_ptr io_context_)
        : Connection{io_context_}, serial{get_io_context()}
    {
    }

    Serial::~Serial()
    {
    }

    std::string Serial::get_port()
    {
        return port;
    }

    void Serial::open(const std::string &port)
    {
        boost::system::error_code ec;
        this->port = port;

        serial.open(port, ec);

        if (ec.failed())
            throw std::runtime_error("Could not open serial port" + this->port + "! " + ec.what());
    }

    void Serial::close()
    {
        boost::system::error_code ec;
        
        serial.close(ec);

        if (ec.failed())
            throw std::runtime_error("Could not close serial port" + this->port + "! " + ec.what());
    }

    void Serial::set_baud_rate(unsigned int baud_rate)
    {
        this->baud_rate = baud_rate;

        boost::asio::serial_port_base::baud_rate b_baud_rate_set;

        serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));

        serial.get_option(b_baud_rate_set);
        if (baud_rate != b_baud_rate_set.value())
            throw std::runtime_error("Could not set baud rate on serial port.");
    }

    void Serial::set_parity(const Parity parity)
    {
        this->parity = parity;
        
        boost::asio::serial_port_base::parity::type b_parity;
        boost::asio::serial_port_base::parity b_parity_set;

        switch (parity)
        {
        case Parity::none:
        {
            b_parity = boost::asio::serial_port_base::parity::none;
        }
        case Parity::odd:
        {
            b_parity = boost::asio::serial_port_base::parity::odd;
        }
        case Parity::even:
        {
            b_parity = boost::asio::serial_port_base::parity::even;
        }
        }

        serial.set_option(boost::asio::serial_port_base::parity(b_parity));

        serial.get_option(b_parity_set);
        if (b_parity != b_parity_set.value())
            throw std::runtime_error("Could not set parity on serial port.");
    }

    void Serial::set_char_size(unsigned int char_size)
    {
        this->char_size = char_size;
        
        boost::asio::serial_port_base::character_size b_char_size_set;

        serial.set_option(boost::asio::serial_port_base::character_size(char_size));

        serial.get_option(b_char_size_set);
        if (char_size != b_char_size_set.value())
            throw std::runtime_error("Could not set character size on serial port.");
    }

    void Serial::set_flow_ctrl(const Flow_ctrl flow_ctrf)
    {
        this->flow_ctrl = flow_ctrf;

        boost::asio::serial_port_base::flow_control::type b_flow_ctrf;
        boost::asio::serial_port_base::flow_control b_flow_ctrf_set;

        switch (flow_ctrf)
        {
        case Flow_ctrl::none:
        {
            b_flow_ctrf = boost::asio::serial_port_base::flow_control::none;
        }
        case Flow_ctrl::software:
        {
            b_flow_ctrf = boost::asio::serial_port_base::flow_control::software;
        }
        case Flow_ctrl::hardware:
        {
            b_flow_ctrf = boost::asio::serial_port_base::flow_control::hardware;
        }
        }

        serial.set_option(boost::asio::serial_port_base::flow_control(b_flow_ctrf));

        serial.get_option(b_flow_ctrf_set);
        if (b_flow_ctrf != b_flow_ctrf_set.value())
            throw std::runtime_error("Could not set flow control on serial port.");
    }

    void Serial::set_stop_bits(const Stop_bits stop_bits)
    {
        this->stop_bits = stop_bits;
        
        boost::asio::serial_port_base::stop_bits::type b_stop_bits;
        boost::asio::serial_port_base::stop_bits b_stop_bits_set;

        switch (stop_bits)
        {
        case Stop_bits::one:
        {
            b_stop_bits = boost::asio::serial_port_base::stop_bits::one;
        }
        case Stop_bits::onepointfive:
        {
            b_stop_bits = boost::asio::serial_port_base::stop_bits::onepointfive;
        }
        case Stop_bits::two:
        {
            b_stop_bits = boost::asio::serial_port_base::stop_bits::two;
        }
        }

        serial.set_option(boost::asio::serial_port_base::stop_bits(b_stop_bits));

        serial.get_option(b_stop_bits_set);
        if (b_stop_bits != b_stop_bits_set.value())
            throw std::runtime_error("Could not set stop bits on serial port.");
    }

    void Serial::set_baud_rate()
    {
        set_baud_rate(this->baud_rate);
    }

    void Serial::set_parity()
    {
        set_parity(this->parity);
    }

    void Serial::set_char_size()
    {
        set_char_size(this->char_size);
    }

    void Serial::set_flow_ctrl()
    {
        set_flow_ctrl(this->flow_ctrl);
    }

    void Serial::set_stop_bits()
    {
        set_stop_bits(this->stop_bits);
    }

    void Serial::write(const std::vector<unsigned char> &data)
    {
        boost::asio::async_write(serial,
                                 boost::asio::buffer(data),
                                 [this](const boost::system::error_code &err, std::size_t write_len)
                                 {
                                     if (err)
                                        error_callback(err.what());
                                     else
                                        write_callback(write_len);
                                 });
    }

    void Serial::run()
    {
        boost::asio::async_read(serial,
                                boost::asio::dynamic_buffer(buffer),
                                [this](const boost::system::error_code &err, std::size_t read_len)
                                {
                                    if (err)
                                        error_callback(err.what());
                                    else
                                        read_callback(buffer.begin(), buffer.end(), read_len);
                                });
    }
}
