#pragma once

#include <Serial.hpp>

template <typename Wc, typename Rc, typename Ec>
class Dyn_serial final : public Serial_port::Serial
{
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
    Dyn_serial(Wc &&wc, Rc &&rc, Ec &&ec);
};

template <typename Wc, typename Rc, typename Ec>
Dyn_serial(Wc &&, Rc &&, Ec &&) -> Dyn_serial<Wc, Rc, Ec>;
