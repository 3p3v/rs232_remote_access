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
        boost::asio::serial_port serial;
        std::string port;

    public:
        Serial(Io_context_ptr io_context_);
        ~Serial() = 0;

        /* Conf */
        std::string get_port();
        virtual void open(const std::string& port) final;
        virtual void close() final;
        virtual void set_baud_rate(unsigned int baud_rate) override final;
        virtual void set_parity(const Parity parity) override final;
        virtual void set_char_size(unsigned int char_size) override final;
        virtual void set_flow_ctrl(const Flow_ctrl flow_ctrf) override final;
        virtual void set_stop_bits(const Stop_bits stop_bits) override final;
        virtual void set_baud_rate() override final;
        virtual void set_parity() override final;
        virtual void set_char_size() override final;
        virtual void set_flow_ctrl() override final;
        virtual void set_stop_bits() override final;
        
        /* Communication */
        virtual void write(const Cont_type &data) final;
        virtual void run() final;
    };
}