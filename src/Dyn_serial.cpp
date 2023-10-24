#pragma once

#include <Dyn_serial.hpp>

template <typename Wc, typename Rc, typename Ec>
void Dyn_serial::write_callback(std::size_t write_len) 
{
    wc(write_len);
}

template <typename Wc, typename Rc, typename Ec>
void Dyn_serial::read_callback(const Data &data, std::size_t write_len) 
{
    rc(data, write_len);
}

template <typename Wc, typename Rc, typename Ec>
void Dyn_serial::error_callback(const unsigned int code, const std::string &err)
{
    ec(code, err);
}