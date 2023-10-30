#pragma once

#include <Serial.hpp>

template <typename Wc, typename Rc, typename Ec>
class Dyn_serial final : public Serial_port::Serial
{
    /* Io_context used by every serial */
    static Io_context_ptr shared_io_context_;
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
      wc{std:::froward<Wc>(wc)},
      rc{std:::froward<Rc>(rc)},
      ec{std:::froward<Ec>(ec)}
{
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
