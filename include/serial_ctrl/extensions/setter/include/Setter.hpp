#pragma once

#include <Common_ext.hpp>
/* Jobs */
#include <Get_set_param_job.hpp>
#include <Change_param_job.hpp>
#include <Param_change_notify_job.hpp>
#include <Param_ready_notify_job.hpp>
#include <Change_postponed_job.hpp>

namespace Logic
{
    template <
        typename Timer_t,
        typename Remote_sett_impl,
        typename Serial_sett_impl>
    class Setter
        : public Common_ext<Timer_t>,
          public std::shared_from_this<Setter>
    {
    public:
        static constexpr params_count{4};

        /////////////////
        /* Definitions */
        /////////////////
    public:
        using Remote_settings_ptr = Mqtt_settings<Remote_sett_impl>;
        using Serial_settings_ptr = Serial_settings<Serial_sett_impl>;
        using Remote_record_ptr = std::shared_ptr<Remote_record>;

        ////////////////////////////////
        /* Data handled inside object */
        ////////////////////////////////
    private:
        /// @brief Remote settings
        Remote_settings_ptr remote_s;
        /// @brief Local settings
        Serial_settings_ptr serial_s;

        /// @brief Port settings, remote state
        Remote_record_ptr remote_rec;

        /// @brief
        void reset_established_params();
        /// @brief
        void add_established_param();
        /// @brief
        void if_all_params_established();
        /// @brief
        void check_if_all_params_established();

        /////////////////
        /* Jobs to add */
        /////////////////
    private:
        /// @brief Connect to remote and serial
        void add_get_set_param_job();
        /// @brief
        void add_change_param_job();
        /// @brief
        void add_change_postponed_job();

        ////////////////////////////////
        /* Actions with other modules */
        ////////////////////////////////
    private:
        /// @brief
        void param_change_notify_job();
        /// @brief
        void param_ready_notify_job();

        ////////////////////////////////////////////
        /* Commands sent to info and set channels */
        ////////////////////////////////////////////
    private:
        /// @brief Get current settings
        void get_settings_();
        /// @brief
        void set_baud_rate_(const Port_settings::Baud_rate arg);
        /// @brief
        void set_parity_(const Port_settings::Parity arg);
        /// @brief
        void set_char_size_(const Port_settings::Char_size arg);
        /// @brief
        void set_stop_bits_(const Port_settings::Stop_bits arg);

        //////////////////////////////////////////////////
        /* Commands received from info and set channels */
        //////////////////////////////////////////////////
        /* Timer sets */
        ////////////////
    private:
        /// @brief Get all current settings
        void get_settings();
        /// @brief
        void get_baud_rate();
        /// @brief
        void get_parity();
        /// @brief
        void get_char_size();
        /// @brief
        void get_stop_bits();
        /// @brief
        void set_baud_rate();
        /// @brief
        void set_parity();
        /// @brief
        void set_char_size();
        /// @brief
        void set_stop_bits();

        //////////////////
        /* Timer resets */
        //////////////////
    private:
        /// @brief
        void set_baud_rate_compl(const std::string &arg);
        /// @brief
        void set_parity_compl(const std::string &arg);
        /// @brief
        void set_char_size_compl(const std::string &arg);
        /// @brief
        void set_stop_bits_compl(const std::string &arg);

        //////////////
        /* External */
        //////////////
    public:
        /// @brief Get all commands that the extension is handling
        /// @return
        Cmds_pack get_cmds() override;

        template <
            typename Remote_settings_ptr_t,
            typename Serial_settings_ptr_t,
            template Remote_record_ptr_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Remote_settings_ptr,
                    std::decay_t<Remote_settings_ptr_t>>>,
            typename = std::enable_if_t<
                std::is_same_v<
                    Serial_settings_ptr,
                    std::decay_t<Serial_settings_ptr_t>>>,
            typename = std::enable_if_t<
                std::is_same_v<
                    Remote_record_ptr,
                    std::decay_t<Remote_record_ptr_t>>>>
        Setter(
            Remote_settings_ptr_t<Remote_sett_impl> &&remote_s,
            Serial_settings_ptr_t<Serial_sett_impl> &&serial_s,
            Remote_record_ptr_t &&remote_rec);
    };

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::reset_established_params()
    {
        remote_rec->params_established = 0;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_established_param()
    {
        remote_rec->params_established++;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::if_all_params_established()
    {
        if (remote_rec->params_established == remote_rec->all_established)
        {
            /* All parameters established, ready to exchange data */
            param_ready_notify_job();
        }
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::check_if_all_params_established()
    {
        return remote_rec->params_established == remote_rec->all_established;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_get_set_param_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Get_set_param_job>(
                [this](auto &job)
                {
                    reset_established_params();

                    /* Get settings if unknown or not configurable */
                    if (!remote_rec->settings_known || !remote_rec->conf_port)
                    {
                        get_settings_();
                    }
                    /* Else send settings */
                    else
                    {
                        /* Try to set parameters set by user */
                        if (!check_for_jobs<Change_param_job>())
                        {
                            /* Send commands to change parameters if user did not sent them in the meantime */
                            set_baud_rate_(remote_rec->port_settings.baud_rate);
                            set_parity_(remote_rec->port_settings.parity);
                            set_char_size_(remote_rec->port_settings.char_size);
                            set_stop_bits_(remote_rec->port_settings.stop_bits);
                        }
                        else
                        {
                            take_job<Change_param_job>();
                        }
                    }
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_change_param_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Change_param_job>(
                [this](auto &job)
                {
                    if (!remote_rec->conf_port)
                    {
                        // TODO tell monitor it is impossible
                    }
                    else
                    {
                        if (if_all_params_established())
                        {
                            reset_established_params();

                            /* Notify other modules about change in connection parameters */
                            param_change_notify_job();

                            /* Send commands to change parameters */
                            set_baud_rate(job.port_settings.baud_rate);
                            set_parity(job.port_settings.parity);
                            set_char_size(job.port_settings.char_size);
                            set_stop_bits(job.port_settings.stop_bits);
                        }
                        else
                        {
                            /* Some other action is being cared of in the moment, postpone changing parameters for later */
                            give_job(Change_postponed_job{job});
                        }
                    }
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_change_postponed_job()
    {
        add_handler(
            Job_type::Trivial,
            Job_excluded::Only_private_queue,
            Job_policies<>::make_job_handler<Change_param_job>(
                [this](auto &job)
                {
                    reset_established_params();

                    /* Notify other modules about change in connection parameters */
                    param_change_notify_job();

                    /* Send commands to change parameters */
                    set_baud_rate(job.port_settings.baud_rate);
                    set_parity(job.port_settings.parity);
                    set_char_size(job.port_settings.char_size);
                    set_stop_bits(job.port_settings.stop_bits);
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::param_change_notify_job()
    {
        forward_job(Param_change_notify_job{});
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::param_ready_notify_job()
    {
        forward_job(Param_ready_notify_job{});
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_settings_()
    {
        remote_s.write_i(
            std::string{get_info_s},
            [serial_ctrl = shared_from_this(), this]()
            {
                get_settings();
            });
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_baud_rate_(const Port_settings::Baud_rate arg)
    {
        remote_s.write_s(
            std::string{set_baud_rate_s},
            std::to_string(arg),
            [serial_ctrl = shared_from_this(), this]()
            {
                set_baud_rate();
            },
            def_ec_callb());
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_parity_(const Port_settings::Parity arg)
    {
        remote_s.write_s(
            std::string{set_parity_s},
            std::to_string(arg),
            [serial_ctrl = shared_from_this(), this]()
            {
                set_parity();
            },
            def_ec_callb());
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_char_size_(const Port_settings::Char_size arg)
    {
        remote_s.write_s(
            std::string{set_char_size_s},
            std::to_string(arg)
                [serial_ctrl = shared_from_this(), this]() {
                    set_char_size();
                },
            def_ec_callb());
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_stop_bits_(const Port_settings::Stop_bits arg)
    {
        remote_s.write_s(
            std::string{set_stop_bits_s},
            std::to_string(arg)
                [serial_ctrl = shared_from_this(), this]() {
                    set_stop_bits();
                },
            def_ec_callb());
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_settings()
    {
        get_baud_rate();
        get_parity();
        get_char_size();
        get_stop_bits();
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_baud_rate()
    {
        timers.start_timer(
            std::string{get_baud_rate_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_parity()
    {
        timers.start_timer(
            std::string{get_paritye_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_char_size()
    {
        timers.start_timer(
            std::string{get_char_size_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_stop_bits()
    {
        timers.start_timer(
            std::string{get_stop_bits_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_baud_rate()
    {
        timers.start_timer(
            std::string{get_baud_rate_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_parity()
    {
        timers.start_timer(
            std::string{get_paritye_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_char_size()
    {
        timers.start_timer(
            std::string{get_char_size_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_stop_bits()
    {
        timers.start_timer(
            std::string{get_stop_bits_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_baud_rate_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{get_baud_rate_s});
        auto arg_ = baud_rate_trans(arg);
        remote_rec->port_settings.baud_rate = arg_;
        serial_s->set_baud_rate(arg_);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_parity_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{get_parity_s});
        auto arg_ = parity_trans(arg);
        remote_rec->port_settings.parity = arg_;
        serial_s->set_parity(arg_);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_char_size_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{get_char_size_s});
        auto arg_ = char_size_trans(arg);
        remote_rec->port_settings.char_size = arg_;
        serial_s->set_char_size(arg_);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_stop_bits_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{get_stop_bits_s});
        auto arg_ = stop_bits_trans(arg);
        remote_rec->port_settings.stop_bits = arg_;
        serial_s->set_stop_bits(arg_);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline Cmds_pack Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_cmds()
    {
        Cmds_pack pack;

        pack.emplace(
            std::string{set_baud_rate_s},
            Command::Policies<Numbers_only>::Dyn_handle(
                [this](const std::string &arg)
                {
                    if (!remote_rec->conf_port)
                    {
                        set_baud_rate();
                    }
                    else
                    {
                        // TODO inform monitor
                    }
                }));

        pack.emplace(
            std::string{get_baud_rate_s},
            Command::Policies<Numbers_only>::Dyn_handle(
                [this](const std::string &arg)
                {
                    if (!remote_rec->conf_port || !check_if_all_params_established())
                    {
                        set_baud_rate_compl(arg);
                    }
                    else
                    {
                        // TODO tell monitor about bad sequence
                        return;
                    }

                    if (remote_rec->conf_port)
                    {
                        add_established_param();
                        if_all_params_established();
                    }
                }));

        pack.emplace(
            std::string{set_parity_s},
            Command::Policies<Alpha_only>::Dyn_handle(
                [this](const std::string &arg)
                {
                    if (!remote_rec->conf_port)
                    {
                        set_parity();
                    }
                    else
                    {
                    }
                }));

        pack.emplace(
            std::string{get_parity_s},
            Command::Policies<Alpha_only>::Dyn_handle(
                [this](const std::string &arg)
                {
                    if (!remote_rec->conf_port || !check_if_all_params_established())
                    {
                        set_parity_compl(arg);
                    }
                    else
                    {
                        // TODO tell monitor about bad sequence
                        return;
                    }

                    if (remote_rec->conf_port)
                    {
                        add_established_param();
                        if_all_params_established();
                    }
                }));

        pack.emplace(
            std::string{set_char_size_s},
            Command::Policies<Numbers_only>::Dyn_handle(
                [this](const std::string &arg)
                {
                    if (!remote_rec->conf_port)
                    {
                        set_char_size();
                    }
                    else
                    {
                    }
                }));

        pack.emplace(
            std::string{get_char_size_s},
            Command::Policies<Numbers_only>::Dyn_handle(
                [this](const std::string &arg)
                {
                    if (!remote_rec->conf_port || !check_if_all_params_established())
                    {
                        set_char_size_compl(arg);
                    }
                    else
                    {
                        // TODO tell monitor about bad sequence
                        return;
                    }

                    if (remote_rec->conf_port)
                    {
                        add_established_param();
                        if_all_params_established();
                    }
                }));

        pack.emplace(
            std::string{set_stop_bits_s},
            Command::Policies<Numbers_only>::Dyn_handle(
                [this](const std::string &arg)
                {
                    if (!remote_rec->conf_port)
                    {
                        set_stop_bits();
                    }
                    else
                    {
                    }
                }));

        pack.emplace(
            std::string{get_stop_bits_s},
            Command::Policies<Numbers_only>::Dyn_handle(
                [this](const std::string &arg)
                {
                    if (!remote_rec->conf_port || !check_if_all_params_established())
                    {
                        set_stop_bits_compl(arg);
                    }
                    else
                    {
                        // TODO tell monitor about bad sequence
                        return;
                    }

                    if (remote_rec->conf_port)
                    {
                        add_established_param();
                        if_all_params_established();
                    }
                }));

        return pack;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    template <typename Remote_settings_ptr_t, typename Serial_settings_ptr_t, typename, typename, typename>
    inline Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::Setter(
        Remote_settings_ptr_t<Remote_sett_impl> &&remote_s,
        Serial_settings_ptr_t<Serial_sett_impl> &&serial_s,
        Remote_record_ptr_t &&remote_rec)
        : remote_s{std::forward<Remote_settings_ptr_t>(remote_s)},
          serial_s{std::forward<Serial_settings_ptr_t>(serial_s)},
          remote_rec{std::forward<Remote_record_ptr_t>(remote_rec)}
    {
        assert("Parameter count not match", params_count == remote_rec->all_established);

        add_get_set_param_job();
        add_change_param_job();
    }

}