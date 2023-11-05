#pragma once

#include <unordered_map>
#include <string>
#include <memory>
#include <Device.hpp>
#include <Ctrl_interface.hpp>
// #include <Serial_settings_intf.hpp>
#include <Serial.hpp>
#include <Flow.hpp>
#include <Monitor.hpp>
// #include <Ip_console.hpp>

namespace Ip_serial
{
    class Console;

    class Ip_serial_ctrl
    {
        const std::shared_ptr<const Device> name;
        const Ip_serial::Console &console;
        const std::unique_ptr<Serial_port::Serial> serial;
        Serial_info info;
        Flow data_in{0};
        Monitor& monitor;

    public:
        bool settings_known;

        template <typename Iter_t>
        void exec(const typename Iter_t begin, const typename Iter_t end);

        template <typename Iter_t>
        void write(const typename Iter_t begin, const typename Iter_t end);

        const Serial_info& get_info() const;
        const Flow get_in() const;

        Ip_serial_ctrl(std::shared_ptr<const Device> name,
                    std::unique_ptr<Serial_port::Serial> &&serial,
                    Ip_serial::Console &console,
                    Serial_info &&info,
                    Monitor& monitor,
                    bool settings_known);
        Ip_serial_ctrl(Ip_serial_ctrl &&) = default;
        Ip_serial_ctrl &operator=(Ip_serial_ctrl &&) = default;
        Ip_serial_ctrl(const Ip_serial_ctrl &) = delete;
        Ip_serial_ctrl &operator=(const Ip_serial_ctrl &) = delete;
        virtual ~Ip_serial_ctrl() = default;
    };

    template <typename Iter_t>
    void Ip_serial_ctrl::exec(const typename Iter_t begin, const typename Iter_t end)
    {
        console.exec(info, begin, end);
        monitor.wake(name);
    }

    template <typename Iter_t>
    void Ip_serial_ctrl::write(const typename Iter_t begin, const typename Iter_t end)
    {
        info->serial.write(begin, end);
        data_in += end - begin;
        monitor.wake(name);
    }
}