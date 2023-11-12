#pragma once

#include <memory>
#include <Base_serial_ctrl.hpp>
#include <Serial_ctrl_helper.hpp>
#include <Mqtt_defs.hpp>
#include <ip_serial/Console.hpp>
#include <impl/Controller.hpp>
#include <Serial_except.hpp>
#include <Mqtt_except.hpp>
#include <Ip_defs.hpp>
#include <Periodic_timer.hpp>
#include <Custom_timer.hpp>
#include <Monitor.hpp>

namespace Ip_serial
{
    class Console;

    /// @brief Object used for mqtt-side communication
    class Ip_serial_ctrl final : public Base_serial_ctrl, protected Mqtt_defs, protected Ip_defs, protected Ip_hi, public std::enable_shared_from_this<Ip_serial_ctrl>
    {
    public:
        using Ip_serial_ctrl_ptr = std::shared_ptr<Ip_serial_ctrl>;

    private:
        const Ip_serial::Console &console;
        Mqtt_port::Impl::Controller &controller;
        Serial_ctrl_helper info;
        std::unique_ptr<Basic_timer> keep_alive_timer{make_periodic_timer([this]()
                                                                          {
                                                                              write_info_no_timer(master_keep_alive_s);
                                                                          },
                                                                          Custom_timer{[serial_ctrl = weak_from_this(), this]()
                                                                          {
                                                                            if (auto serial = serial_ctrl.lock())
                                                                            {
                                                                                Monitor::get().error(Exception::Cmds_except{"Keep alive timed out, retrying if enabled..."});
                                                                                /* Try to say hi to device again */
                                                                                say_hi_();
                                                                            }
                                                                          }})};

        void say_hi_();

    public:
        /// @brief Process received command
        template <typename Iter_t>
        void exec(Iter_t begin, Iter_t end);

        /// @brief Write to serial
        template <typename Cont_t, typename Ok_callb>
        void write(const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb);

        /// @brief Write message to info channel
        /// @tparam Cont_t
        /// @param msg
        template <typename Cont_t>
        void write_info(Cont_t &&msg);

        template <typename Cont_t>
        void write_info_no_timer(Cont_t &&msg);

        template <typename Cont_t, typename Callb>
        void write_info_custom(Cont_t &&msg, Callb &&callb);

        template <typename Cont_t, typename Arg_cont_t>
        void write_info(Cont_t &&msg, Arg_cont_t &&arg);

        /// @brief Connect to data and info channels
        void connect();

        /// @brief Get info pack
        /// @return
        const Serial_info &get_info() const;

        Serial_ctrl_helper &get_helper();

        /* Init */
        void say_hi();
        void say_hi_compl();
        void keep_alive();

        /* Get current settings */
        void get_settings();

        /* Set param initializer */
        void set_baud_rate(const unsigned int baud_rate);
        void set_parity(const Serial_port::Ctrl_defs::Parity parity);
        void set_char_size(const unsigned int char_size);
        void set_flow_ctrl(const Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl);
        void set_stop_bits(const Serial_port::Ctrl_defs::Stop_bits stop_bits);

        /* Wait for params */
        void set_baud_rate();
        void set_parity();
        void set_char_size();
        void set_flow_ctrl();
        void set_stop_bits();

        /* Set param completer */
        void set_baud_rate_compl(const std::string &arg);
        void set_parity_compl(const std::string &arg);
        void set_char_size_compl(const std::string &arg);
        void set_flow_ctrl_compl(const std::string &arg);
        void set_stop_bits_compl(const std::string &arg);

        Ip_serial_ctrl(Base_serial_ctrl &&base,
                       Mqtt_port::Impl::Controller &controller,
                       std::shared_ptr<Serial_port::Serial> &&serial,
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
        try
        {
            /* Interpret received data */
            console.exec(*this, begin, end);
            monitor.wake(device);
        }
        catch (const Exception::Exception &e)
        {
            monitor.error(e);
        }
    }

    template <typename Cont_t, typename Ok_callb>
    void Ip_serial_ctrl::write(const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb)
    {
        try
        {
            flow += end - begin;
            info.serial->write<Cont_t>(begin, end, std::forward<Ok_callb>(ok_callb));
            monitor.wake(device);
        }
        catch (const boost::exception &e)
        {
            monitor.error(Exception::Serial_except{e});
        }
    }

    template <typename Cont_t>
    void Ip_serial_ctrl::write_info(Cont_t &&msg)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        auto callb = [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                      serial_ctrl = shared_from_this(),
                      this](size_t size)
        {
            info.timers.start_timer(msg_ptr->data());
        };

        try
        {
            controller.write(device->get_info_ch(),
                             qos,
                             beg,
                             end,
                             std::forward<decltype(callb)>(callb),
                             [&monitor = this->monitor](int)
                             {
                                 monitor.error(Exception::Mqtt_write_except{});
                             });
        }
        catch (const mqtt::exception &e)
        {
            monitor.error(Exception::Mqtt_except{e});
        }
    }

    template <typename Cont_t>
    void Ip_serial_ctrl::write_info_no_timer(Cont_t &&msg)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        auto callb = [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr)](size_t size) {
        };

        try
        {
            controller.write(device->get_info_ch(),
                             qos,
                             beg,
                             end,
                             std::forward<decltype(callb)>(callb),
                             [](int)
                             {
                                 Monitor::get().error(Exception::Mqtt_write_except{});
                             });
        }
        catch (const mqtt::exception &e)
        {
            Monitor::get().error(Exception::Mqtt_except{e});
        }
    }

    template <typename Cont_t, typename Callb>
    void Ip_serial_ctrl::write_info_custom(Cont_t &&msg, Callb &&callb)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        auto callb = [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                      msg = std::string{msg},
                      callb = std::forward<Callb>(callb),
                      serial_ctrl = shared_from_this(),
                      this](size_t size)
        {
            info.timers.start_timer(msg, callb);
        };

        try
        {
            controller.write(device->get_info_ch(),
                             qos,
                             beg,
                             end,
                             std::forward<decltype(callb)>(callb),
                             [&monitor = this->monitor](int)
                             {
                                 monitor.error(Exception::Mqtt_write_except{});
                             });
        }
        catch (const mqtt::exception &e)
        {
            monitor.error(Exception::Mqtt_except{e});
        }
    }

    template <typename Cont_t, typename Arg_cont_t>
    void Ip_serial_ctrl::write_info(Cont_t &&msg, Arg_cont_t &&arg)
    {
        auto msg_ptr = std::make_unique<std::string>(std::string{msg} + " " + std::string{arg} + "\n");

        auto beg = msg_ptr->cbegin();
        auto end = msg_ptr->cend();

        auto callb = [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                      msg = std::string{msg},
                      serial_ctrl = shared_from_this(),
                      this](size_t size)
        {
            info.timers.start_timer(msg);
        };

        try
        {
            controller.write(device->get_info_ch(),
                             qos,
                             beg,
                             end,
                             std::forward<decltype(callb)>(callb),
                             [&monitor = this->monitor](int)
                             {
                                 monitor.error(Exception::Mqtt_write_except{});
                             });
        }
        catch (const mqtt::exception &e)
        {
            monitor.error(Exception::Mqtt_except{e});
        }
    }
}