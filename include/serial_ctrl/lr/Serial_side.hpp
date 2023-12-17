#pragma once

#include <memory>
#include <stdexcept>
#include <Serial_except.hpp>
#include <Dyn_serial.hpp>
#include <Ip_master.hpp>

class Serial_side
{
    const std::string &com;
    std::unique_ptr<Serial_port::Serial> &serial;

public:
    /// @brief Write to serial, use callbacks
    /// @tparam Cont_t
    /// @tparam Ok_callb
    /// @tparam Ec_callb
    /// @param begin
    /// @param end
    /// @param ok_callb
    /// @param ec_callb
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    void write(const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

    /// @brief
    /// @tparam Cont_t
    /// @tparam Ok_callb
    /// @tparam Ec_callb
    /// @param com
    /// @param begin
    /// @param end
    /// @param ok_callb
    /// @param ec_callb
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    Serial_side(const std::string &com,
                std::unique_ptr<Serial_port::Serial> &serial,
                const typename Cont_t::const_iterator begin,
                const typename Cont_t::const_iterator end,
                Ok_callb &&ok_callb,
                Ec_callb &&ec_callb);
};

template <typename Cont_t, typename Ok_callb, typename Ec_callb>
inline void Serial_side::write(const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
{
    try
    {
        serial->write<Cont_t>(
            begin,
            end,
            [ok_callb = std::forward<Ok_callb>(ok_callb), ec_callb = std::forward<Ec_callb>(ec_callb)](auto &ec, auto write_len)
            {
                if (ec == 0)
                {
                    ok_callb(write_len);
                }
                else
                {
                    ec_callb(ec);
                } });
    }
    catch (const boost::exception &e)
    {
        throw Exception::Serial_except{e.what()};
    }
}

template <typename Cont_t, typename Ok_callb, typename Ec_callb>
Serial_side::Serial_side(const std::string &com,
                         std::unique_ptr<Serial_port::Serial> &serial,
                         const typename Cont_t::const_iterator begin,
                         const typename Cont_t::const_iterator end,
                         Ok_callb &&ok_callb,
                         Ec_callb &&ec_callb)
    : com{com}, serial{serial}
{
    if (serial == nullptr)
    {
        /* Create serial, set callbacks */
        serial = std::make_unique<Serial_port::Serial>(
            com,
            [ok_callb = std::forward<Ok_callb>(ok_callb), this](auto data, auto read_len)
            {
                ok_callb(data, data + read_len, [this](auto begin, auto end)
                         {
                    /* Resume reading */
                    serial->listen(begin, end - begin); });
            },
            [ec_callb = std::forward<Ec_callb>(ec_callb)](const unsigned int code, const std::string &err)
            {
                ec_callb(Exception::Serial_except{err});
            });

        try
        {
            /* Run serial */
            serial->run<Cont_t>(begin, end);
        }
        catch (const boost::exception &e)
        {
            throw Exception::Serial_except{e.what()};
        }
    }
}