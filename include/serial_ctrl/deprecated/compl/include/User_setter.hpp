#pragma once

#include <memory>
#include <Remote_record.hpp>
#include <Set_defs.hpp>
#include <Param_defs.hpp>

using namespace Serial_port;

namespace Logic
{
    /// @brief Declares operations that can be done by a user
    template <typename Mqtt, typename Serial>
    class User_setter : protected Set_defs,
                        protected Param_defs
    {
        using Remote = Remote_record<Mqtt, Serial>;
        using Shared_remote = std::shared_ptr<Remote>;

        /// @brief Controllable equipment and saved data
        Shared_remote remote;

    public:
        /// @brief
        void set_baud_rate(const unsigned int baud_rate);
        /// @brief
        void set_parity(const Ctrl_defs::Parity parity);
        /// @brief
        void set_char_size(const unsigned int char_size);
        /// @brief
        void set_stop_bits(const Ctrl_defs::Stop_bits stop_bits);

        User_setter(Shared_remote remote);
    };

    template <typename Mqtt, typename Serial>
    User_setter::User_setter(Shared_remote remote)
        : remote{std::move(remote)}
    {
        /* Add commands used to control UART settings */
        add_cmd(std::string{set_baud_rate_s},
                Command::Policies<Numbers_only>::Dyn_handle([](const std::string &arg)
                                                            { remote->mqtt.timers.start_timer(set_baud_rate_s.data()); }));
        add_cmd(std::string{get_baud_rate_s},
                Command::Policies<Numbers_only>::Dyn_handle([](const std::string &arg)
                                                            { helper.set_baud_rate_compl(arg); }));

        add_cmd(std::string{set_parity_s},
                Command::Policies<Alpha_only>::Dyn_handle([](const std::string &arg)
                                                          { remote->mqtt.timers.start_timer(set_baud_rate_s.data()); }));
        add_cmd(std::string{get_parity_s},
                Command::Policies<Alpha_only>::Dyn_handle([](const std::string &arg)
                                                          { helper.set_parity_compl(arg); }));

        add_cmd(std::string{set_char_size_s},
                Command::Policies<Numbers_only>::Dyn_handle([](const std::string &arg)
                                                            { remote->mqtt.timers.start_timer(set_baud_rate_s.data()); }));
        add_cmd(std::string{get_char_size_s},
                Command::Policies<Numbers_only>::Dyn_handle([](const std::string &arg)
                                                            { helper.set_char_size_compl(arg); }));

        add_cmd(std::string{set_stop_bits_s},
                Command::Policies<Numbers_only>::Dyn_handle([](const std::string &arg)
                                                            { remote->mqtt.timers.start_timer(set_baud_rate_s.data()); }));
        add_cmd(std::string{get_stop_bits_s},
                Command::Policies<Numbers_only>::Dyn_handle([](const std::string &arg)
                                                            { helper.set_stop_bits_compl(arg); }));
    }

    template <typename Mqtt, typename Serial>
    void User_setter::set_baud_rate(const unsigned int baud_rate)
    {
        remote->mqtt.set(
            set_baud_rate_s.data(),
            baud_rate_trans(baud_rate),
            [remote]()
            {
                remote->mqtt.timers.start_timer(set_baud_rate_s.data());
            });
    }

    template <typename Mqtt, typename Serial>
    void User_setter::set_parity(const Serial_port::Ctrl_defs::Parity parity)
    {
        remote->mqtt.set(
            parity_s.data(),
            parity_trans(parity),
            [remote]()
            {
                remote->mqtt.timers.start_timer(parity_s.data());
            });
    }

    template <typename Mqtt, typename Serial>
    void User_setter::set_char_size(const unsigned int char_size)
    {
        remote->mqtt.set(
            char_size_s.data(),
            char_size_trans(char_size),
            [remote]()
            {
                remote->mqtt.timers.start_timer(char_size_s.data());
            });
    }

    template <typename Mqtt, typename Serial>
    void User_setter::set_stop_bits(const Serial_port::Ctrl_defs::Stop_bits stop_bits)
    {
        remote->mqtt.set(
            stop_bits_s.data(),
            stop_bits_trans(stop_bits),
            [remote]()
            {
                remote->mqtt.timers.start_timer(stop_bits_s.data());
            });
    }
}