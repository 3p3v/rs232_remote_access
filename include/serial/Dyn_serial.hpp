#pragma once

#include <Serial.hpp>
#include <atomic>
#include <mutex>
#include <Serial_context.hpp>


namespace Phy_serial
{
    template <typename Wc, typename Rc, typename Ec>
    class Dyn_serial final : public Serial_port::Serial, public Serial_context
    {
        /* Callback's definitions */
        Wc wc;
        Rc rc;
        Ec ec;

    protected:
        /* Callbacks */
        void write_callback(std::size_t write_len) override;
        void read_callback(const Data &data, std::size_t read_len) override;
        void error_callback(const unsigned int code, const std::string &err) override;

    public:
        template <typename Str>
        Dyn_serial(Str &&port, Wc &&wc, Rc &&rc, Ec &&ec);
    };

    template <typename Wc, typename Rc, typename Ec, typename Str>
    Dyn_serial(Str &&, Wc &&, Rc &&, Ec &&) -> Dyn_serial<Wc, Rc, Ec>;

    template <typename Wc, typename Rc, typename Ec>
    template <typename Str>
    Dyn_serial<Wc, Rc, Ec>::Dyn_serial(Str &&port, Wc &&wc, Rc &&rc, Ec &&ec)
        : Serial{shared_io_context_, std::forward<Str>(port)},
        wc{std::forward<Wc>(wc)},
        rc{std::forward<Rc>(rc)},
        ec{std::forward<Ec>(ec)}
    {
        // std::lock_guard<std::mutex> guard(run_guard);
    }

    template <typename Wc, typename Rc, typename Ec>
    void Dyn_serial<Wc, Rc, Ec>::write_callback(std::size_t write_len) 
    {
        wc(write_len);
    }

    template <typename Wc, typename Rc, typename Ec>
    void Dyn_serial<Wc, Rc, Ec>::read_callback(const Data &data, std::size_t write_len) 
    {
        rc(data, write_len);
    }

    template <typename Wc, typename Rc, typename Ec>
    void Dyn_serial<Wc, Rc, Ec>::error_callback(const unsigned int code, const std::string &err)
    {
        ec(code, err);
    }
}