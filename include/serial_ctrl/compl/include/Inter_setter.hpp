#pragma once

#include <Ctrl_interface.hpp>
#include <Ext_setter.hpp>

using namespace Serial_port;

namespace Logic
{
    /// @brief Declares operations that can be done by a user
    class User_setter : public Ext_setter
    {
    public:
        public:
        /* Init */
        /// @brief
        void say_hi_();

    protected:
        /* Keep alive */
        std::unique_ptr<Basic_timer> keep_alive_timer{
            std::make_unique<Periodic_timer>(
                /* If don't get any message from the device in time */
                [ptr = weak_from_this_(), this]()
                { mqtt.write_i(
                      master_keep_alive_s.data(),
                      [](size_t) {},
                      [](const std::exception &) {}); },
                /* If don't get response to above message */
                Custom_timer{
                    [ptr = weak_from_this_(), this]()
                    {
                        if (auto serial = ptr.lock())
                        {
                            Monitor::get().error(Exception::Cmds_timeout{"Keep alive timed out, retrying if enabled..."});
                            /* Try to say hi to device again */
                            say_hi_();
                        }
                    }})};
        /// @brief If started keep alive
        std::atomic_bool keep_alive_started{false};
        /// @brief Timers for every other command
        Timer_cont timers;

    protected:
        void set_baud_rate_() override final;
        /// @brief
        void set_parity_() override final;
        /// @brief
        void set_char_size_() override final;
        /// @brief
        void set_stop_bits_() override final;

    public:
        /* Init */
        /// @brief
        void say_hi_compl();
        /// @brief
        void keep_alive_start();
        /// @brief
        void keep_alive();
        /// @brief
        void keep_alive_stop();
        /// @brief
        void clear_timers();
        /// @brief
        void reboot();

        /// @brief Get current settings
        void get_settings();

        /* Wait for params */
        /// @brief
        void set_baud_rate();
        /// @brief
        void set_parity();
        /// @brief
        void set_char_size();
        /// @brief
        void set_stop_bits();

        /* Set param completer */
        /// @brief
        void set_baud_rate_compl(const std::string &arg);
        /// @brief
        void set_parity_compl(const std::string &arg);
        /// @brief
        void set_char_size_compl(const std::string &arg);
        /// @brief
        void set_stop_bits_compl(const std::string &arg);
    };
}