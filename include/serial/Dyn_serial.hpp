#pragma once

#include <Serial.hpp>
#include <atomic>
#include <mutex>
#include <Serial_context.hpp>


namespace Phy_serial
{
    template <typename Rc, typename Ec>
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
        Dyn_serial(Str &&port, Rc &&rc, Ec &&ec);
    };

    template <typename Rc, typename Ec, typename Str>
    Dyn_serial(Str &&, Rc &&, Ec &&) -> Dyn_serial<Rc, Ec>;

    template <typename Rc, typename Ec>
    template <typename Str>
    Dyn_serial<Rc, Ec>::Dyn_serial(Str &&port, Rc &&rc, Ec &&ec)
        : Serial{shared_io_context_, std::forward<Str>(port)},
        rc{std::forward<Rc>(rc)},
        ec{std::forward<Ec>(ec)}
    {
        // std::lock_guard<std::mutex> guard(run_guard);
    }

    template <typename Rc, typename Ec>
    void Dyn_serial<Rc, Ec>::read_callback(const Data &data, std::size_t write_len) 
    {
        rc(data, write_len);
    }

    template <typename Rc, typename Ec>
    void Dyn_serial<Rc, Ec>::error_callback(const unsigned int code, const std::string &err)
    {
        ec(code, err);
    }
}