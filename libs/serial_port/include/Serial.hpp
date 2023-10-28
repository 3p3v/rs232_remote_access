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
    private:
        Serial(Serial &) = delete;
        Serial& operator=(Serial &) = delete;

        boost::asio::serial_port serial;
        std::string port;

    public:
        Serial(Io_context_ptr io_context_);
        Serial(Serial &&) = default;
        Serial& operator=(Serial &&) = default;
        virtual ~Serial() = 0;

        /* Conf */
        std::string get_port();
        void open(const std::string& port);
        void close();
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
        void write(const Data &data) override final;
        void run() override final;
    };
}