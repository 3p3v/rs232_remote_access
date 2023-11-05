#pragma once

#include <Base_serial_ctrl.hpp>
// #include <Ip_console.hpp>

namespace Ip_serial
{
    class Console;

    class Serial_ctrl : public Base_serial_ctrl
    {
        const std::string com1;
        const std::string com2;
        const Mqtt_port::Impl::Controller &controller;

    public:
        template <typename Iter_t>
        void write(const typename Iter_t begin, const typename Iter_t end);

        std::pair<const std::string&, const std::string&> get_coms() const;

        Serial_ctrl(std::shared_ptr<const Device> name,
                    Mqtt_port::Impl::Controller &controller,
                    Monitor& monitor, 
                    bool write_access, 
                    Mqtt_port::Impl::Controller &controller,
                    std::string &&com1,
                    std::string &&com2);
        Serial_ctrl(Base_serial_ctrl &&base,
                    Mqtt_port::Impl::Controller &controller,
                    std::string &&com1,
                    std::string &&com2);
    };

    template <typename Iter_t>
    void Base_serial_ctrl::write(const typename Iter_t begin, const typename Iter_t end)
    {
        controller.write(channel_name, begin, end);
        data_out += end - begin;
        monitor.wake(name);
    }
}