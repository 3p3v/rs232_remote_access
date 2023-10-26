#pragma once

#include <Dyn_serial.hpp>

template <typename Wc, typename Rc, typename Ec>
Dyn_serial<Wc, Rc, Ec>::Dyn_serial(Wc &&wc, Rc &&rc, Ec &&ec)
    : io_context_{shared_io_context_},
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