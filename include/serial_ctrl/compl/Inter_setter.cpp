#include <Inter_setter.hpp>

namespace Logic
{
    void Inter_setter::say_hi_()
    {
        write_i(
            master_hi_s.data(),
            [serial_ctrl = weak_from_this()]()
            {
                if (auto serial = serial_ctrl.lock())
                {
                    Monitor::get().error(Exception::Cmds_timeout{"Saying hi timed out, retrying if enabled..."});
                    /* Try to say hi to device again */
                    serial->say_hi_();
                }
            });
    }

    void Inter_setter::say_hi_compl()
    {
        timers.stop_timer(master_hi_s.data());
    }

    void Inter_setter::keep_alive_start()
    {
        keep_alive_started = true;
        keep_alive();
    }

    void Inter_setter::keep_alive()
    {
        if (keep_alive_started)
            keep_alive_timer->start();
    }

    void Inter_setter::keep_alive_stop()
    {
        keep_alive_started = false;
        keep_alive_timer->stop();
    }

    void Inter_setter::clear_timers()
    {
        timers.clear();
    }

    void Inter_setter::reboot()
    {
        keep_alive_stop();
        clear_timers();
        say_hi_();
    }

    void Inter_setter::get_settings()
    {
        auto callb = [msg_ptr = std::forward<decltype(msg_ptr)>(msg_ptr),
                      serial_ctrl = shared_from_this(),
                      this]()
        {
            set_baud_rate();
            set_parity();
            set_char_size();
            set_stop_bits();
        };

        try
        {
            write_i(get_info_s.data(),
                    std::forward<decltype(callb)>(callb));
        }
        catch (const mqtt::exception &e)
        {
            Monitor::get().error(Exception::Mqtt_except{e});
        }
    }

    void Inter_setter::set_baud_rate()
    {
        timers.start_timer(set_baud_rate_s.data());
    }

    void Inter_setter::set_parity()
    {
        timers.start_timer(set_parity_s.data());
    }

    void Inter_setter::set_char_size()
    {
        timers.start_timer(set_char_size_s.data());
    }

    void Inter_setter::set_stop_bits()
    {
        timers.start_timer(set_stop_bits_s.data());
    }

    void Inter_setter::set_baud_rate_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{set_baud_rate_s});
        auto arg_ = baud_rate_trans(arg);
        info.baud_rate = arg_;
        serial->set_baud_rate(arg_);
    }

    void Inter_setter::set_parity_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{set_parity_s});
        auto arg_ = parity_trans(arg);
        info.parity = arg_;
        serial->set_parity(arg_);
    }

    void Inter_setter::set_char_size_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{set_char_size_s});
        auto arg_ = char_size_trans(arg);
        info.char_size = arg_;
        serial->set_char_size(arg_);
    }

    void Inter_setter::set_stop_bits_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{set_stop_bits_s});
        auto arg_ = stop_bits_trans(arg);
        info.stop_bits = arg_;
        serial->set_stop_bits(arg_);
    }
}