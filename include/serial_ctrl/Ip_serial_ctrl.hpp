#pragma once

#include <memory>
#include <Base_serial_ctrl.hpp>
#include <Serial_ctrl_helper.hpp>
#include <Mqtt_defs.hpp>
#include <ip_serial/Console.hpp>
#include <impl/Controller.hpp>
#include <Rec_callb_intf.hpp>

namespace Ip_serial
{
    class Console;

    // namespace Mqtt_port
    // {
    //     namespace Impl
    //     {
    //         class Controller;
    //     }
    // }

    /// @brief Object used for mqtt-side communication
    class Ip_serial_ctrl final : public Base_serial_ctrl, protected Mqtt_defs, public std::enable_shared_from_this<Ip_serial_ctrl>
    {
    public:
        using Ip_serial_ctrl_ptr = std::shared_ptr<Ip_serial_ctrl>;

    private:
        const Ip_serial::Console &console; // 2
        Mqtt_port::Impl::Controller &controller;
        Serial_ctrl_helper info;

    public:
        /// @brief Process received command
        template <typename Iter_t>
        void exec(Iter_t begin, Iter_t end);

        /// @brief Write to serial
        template <typename Iter_t, typename Ok_callb>
        void write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb);

        /// @brief Write message to info channel
        /// @tparam Cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param msg
        /// @param ok_callb
        /// @param ec_callb
        template <typename Cont_t, typename Ok_callb>
        void write_info(Cont_t &&msg, Ok_callb &&ok_callb);

        /// @brief Connect to data and info channels
        void connect();

        /// @brief Get info pack
        /// @return
        const Serial_info &get_info() const;

        Ip_serial_ctrl(Base_serial_ctrl &&base,
                       Mqtt_port::Impl::Controller &controller,
                       std::unique_ptr<Serial_port::Serial> &&serial,
                       Ip_serial::Console &console,
                       Serial_info &&info,
                       bool settings_known);
        Ip_serial_ctrl(Base_serial_ctrl &&base,
                       Mqtt_port::Impl::Controller &controller,
                       Serial_ctrl_helper &&info,
                       Ip_serial::Console &console);
        Ip_serial_ctrl(Ip_serial_ctrl &&) = default;
        Ip_serial_ctrl &operator=(Ip_serial_ctrl &&) = default;
        Ip_serial_ctrl(const Ip_serial_ctrl &) = delete;
        Ip_serial_ctrl &operator=(const Ip_serial_ctrl &) = delete;
        ~Ip_serial_ctrl();
    };

    template <typename Iter_t>
    void Ip_serial_ctrl::exec(Iter_t begin, Iter_t end)
    {
        console.exec(info, begin, end);
        monitor.wake(device);
    }

    template <typename Iter_t, typename Ok_callb>
    void Ip_serial_ctrl::write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb);
    {
        flow += end - begin;
        info.serial->write(begin, end, std::forward<Ok_callb>(ok_callb));
        monitor.wake(device);
    }

    template <typename Cont_t, typename Ok_callb>
    void Ip_serial_ctrl::write_info(Cont_t &&msg, Ok_callb &&ok_callb)
    {
        auto msg_ptr = std::make_unique<Cont_t>(std::move(msg));

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        auto callb_wrap = [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                           ok_callb = std::forward<Ok_callb>(ok_callb)](size_t size)
        {
            ok_callb(size);
        };

        controller.write(device->get_info_ch(),
                         qos,
                         beg,
                         end,
                         std::forward<decltype(callb_wrap)>(callb_wrap),
                         [](int) {
                            // send error to monitor
                         });
    }
}