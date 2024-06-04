#pragma once

#include <Common_ext.hpp>
#include <Mqtt_settings.hpp>
#include <Serial_settings.hpp>
#include <Remote_record.hpp>
#include <Set_defs.hpp>
#include <Get_defs.hpp>
/* Policies */
#include <Numbers_only.hpp>
#include <Alpha_only.hpp>
/* Jobs */
#include <Get_set_param_job.hpp>
#include <Change_param_job.hpp>
#include <Param_change_notify_job.hpp>
#include <Param_ready_notify_job.hpp>
#include <Change_postponed_job.hpp>
/**/
#include <memory>
/**/
#include <Setter_except.hpp>
#include <Timeout_except.hpp>

namespace Logic
{
    template <
        typename Timer_t,
        typename Remote_sett_impl,
        typename Serial_sett_impl>
    class Setter
        : public Common_ext<Timer_t>
    {
    public:
        static constexpr auto params_count{4};

        /////////////////
        /* Definitions */
        /////////////////
    public:
        using Remote_settings_ptr = Mqtt_settings<Remote_sett_impl>;
        using Serial_settings_ptr = Serial_settings<Serial_sett_impl>;

        ////////////////////////////////
        /* Data handled inside object */
        ////////////////////////////////
    private:
        /// @brief Remote settings
        Remote_settings_ptr remote_s;
        /// @brief Local settings
        Serial_settings_ptr serial_s;

        /// @brief Port settings, remote state
        Remote_record &remote_rec;

        /// @brief
        void reset_established_params();
        /// @brief
        void add_established_param();
        /// @brief
        void if_all_params_established();
        /// @brief
        bool check_if_all_params_established();
        /// @brief
        void activate_record();
        /// @brief
        void deactivate_record();
        /// @brief
        bool if_record_active();

        /////////////////
        /* Jobs to add */
        /////////////////
    protected:
        /// @brief Add ability to restart record
        void add_restart_job() override;

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
        Cmds_pack return_cmds() override;

        template <
            typename Device_weak_ptr_t,
            typename Remote_settings_ptr_t,
            typename Serial_settings_ptr_t>
        Setter(
            Forwarder &&manager,
            Notyfier &&notyfier,
            Device_weak_ptr_t &&device_ptr,
            Remote_settings_ptr_t &&remote_s,
            Serial_settings_ptr_t &&serial_s,
            Remote_record &remote_rec);

        Setter(Setter &&) = delete;
        Setter &operator=(Setter &&) = delete;
        Setter(const Setter &) = delete;
        Setter &operator=(const Setter &) = delete;

        template <typename... Args_t>
        static auto make(Args_t &&...args);
    };

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::reset_established_params()
    {
        remote_rec.params_established = 0;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_established_param()
    {
        remote_rec.params_established++;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::if_all_params_established()
    {
        if (remote_rec.params_established == remote_rec.all_established)
        {
            /* All parameters established, ready to exchange data */
            if (!check_for_jobs<Change_postponed_job>())
            {
                /* Parameters weren't changed again, it's safe to inform others about established connection */
                param_ready_notify_job();
            }
            else
            {
                take_most_recent_job_and_clear<Change_postponed_job>();
            }
        }
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline bool Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::check_if_all_params_established()
    {
        return remote_rec.params_established == remote_rec.all_established;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::activate_record()
    {
        remote_rec.record_active = true;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::deactivate_record()
    {
        remote_rec.record_active = false;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline bool Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::if_record_active()
    {
        return remote_rec.record_active;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_restart_job()
    {
        // Add job for resetting all timers (used when there was error in communication with device)
        override_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Restart_job>(
                [this](auto &job)
                {
                    /* Clear all timers */
                    timers.clear();

                    deactivate_record();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_get_set_param_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Get_set_param_job>(
                [this](auto &job)
                {
                    /* Can receive messages from now on */
                    activate_record();
                    reset_established_params();

                    /* Get settings if unknown or not configurable */
                    if (!remote_rec.settings_known || remote_rec.conf_port == Remote_conf_port::Non_configurable)
                    {
                        get_settings_();
                    }
                    /* Else send settings */
                    else
                    {
                        /* Try to set parameters set by user */
                        if (!check_for_jobs<Change_postponed_job>())
                        {
                            /* Send commands to change parameters if user did not sent them in the meantime */
                            set_baud_rate_(remote_rec.port_settings.baud_rate);
                            set_parity_(remote_rec.port_settings.parity);
                            set_char_size_(remote_rec.port_settings.char_size);
                            set_stop_bits_(remote_rec.port_settings.stop_bits);
                        }
                        else
                        {
                            take_most_recent_job_and_clear<Change_postponed_job>();
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
                    if (remote_rec.conf_port == Remote_conf_port::Non_configurable)
                    {
                        notyfier.debug("Cannot change parameters as port is set as non-configuable...");
                    }
                    else
                    {
                        /* If any jobs were given in the mean time - delete them */
                        remove_jobs<Change_postponed_job>();
                        
                        /* Change params only if they were established previously, there is no way to interrupt action now */
                        if (check_if_all_params_established())
                        {
                            notyfier.debug("Received request to change connection parameters...");
                            
                            reset_established_params();

                            /* Notify other modules about a change in connection parameters */
                            param_change_notify_job();

                            /* Send commands to change parameters */
                            set_baud_rate_(job.port_settings.baud_rate);
                            set_parity_(job.port_settings.parity);
                            set_char_size_(job.port_settings.char_size);
                            set_stop_bits_(job.port_settings.stop_bits);
                        }
                        else
                        {
                            notyfier.debug("Changing connection parameters will be executed after current action...");
                            
                            /* Some other action is being taken care of at the moment, postpone changing parameters for later */
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
            Job_policies<>::make_job_handler<Change_postponed_job>(
                [this](auto &job)
                {
                    if (remote_rec.conf_port == Remote_conf_port::Non_configurable)
                    {
                        notyfier.debug("Cannot change parameters as port is set as non-configuable...");
                    }
                    else
                    {
                        notyfier.debug("Changing connection parameters to these specyfied by user...");
                        
                        reset_established_params();

                        /* Notify other modules about change in connection parameters */
                        param_change_notify_job();

                        /* Send commands to change parameters */
                        set_baud_rate_(job.port_settings.baud_rate);
                        set_parity_(job.port_settings.parity);
                        set_char_size_(job.port_settings.char_size);
                        set_stop_bits_(job.port_settings.stop_bits);
                    }
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
            std::string{Get_defs::get_info_s},
            [serial_ctrl = shared_from_this(), this]()
            {
                get_settings();
            },
            def_ec_callb());
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_baud_rate_(const Port_settings::Baud_rate arg)
    {
        remote_s.write_s(
            std::string{Set_defs::set_baud_rate_s},
            Set_defs::baud_rate_trans(arg),
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
            std::string{Set_defs::set_parity_s},
            Set_defs::parity_trans(arg),
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
            std::string{Set_defs::set_char_size_s},
            Set_defs::char_size_trans(arg),
            [serial_ctrl = shared_from_this(), this]()
            {
                set_char_size();
            },
            def_ec_callb());
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_stop_bits_(const Port_settings::Stop_bits arg)
    {
        remote_s.write_s(
            std::string{Set_defs::set_stop_bits_s},
            Set_defs::stop_bits_trans(arg),
            [serial_ctrl = shared_from_this(), this]()
            {
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
            std::string{Get_defs::get_baud_rate_s},
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_parity()
    {
        timers.start_timer(
            std::string{Get_defs::get_parity_s},
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_char_size()
    {
        timers.start_timer(
            std::string{Get_defs::get_char_size_s},
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_stop_bits()
    {
        timers.start_timer(
            std::string{Get_defs::get_stop_bits_s},
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_baud_rate()
    {
        timers.start_timer(
            std::string{Get_defs::get_baud_rate_s},
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_parity()
    {
        timers.start_timer(
            std::string{Get_defs::get_parity_s},
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_char_size()
    {
        timers.start_timer(
            std::string{Get_defs::get_char_size_s},
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_stop_bits()
    {
        timers.start_timer(
            std::string{Get_defs::get_stop_bits_s},
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                }));
    }
    
    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_baud_rate_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{Get_defs::get_baud_rate_s});
        auto arg_ = Set_defs::baud_rate_trans(arg);
        remote_rec.port_settings.baud_rate = arg_;
        serial_s.set_baud_rate(arg_);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_parity_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{Get_defs::get_parity_s});
        auto arg_ = Set_defs::parity_trans(arg);
        remote_rec.port_settings.parity = arg_;
        serial_s.set_parity(arg_);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_char_size_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{Get_defs::get_char_size_s});
        auto arg_ = Set_defs::char_size_trans(arg);
        remote_rec.port_settings.char_size = arg_;
        serial_s.set_char_size(arg_);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::set_stop_bits_compl(const std::string &arg)
    {
        timers.stop_timer(std::string{Get_defs::get_stop_bits_s});
        auto arg_ = Set_defs::stop_bits_trans(arg);
        remote_rec.port_settings.stop_bits = arg_;
        serial_s.set_stop_bits(arg_);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline Remote_ext_base::Cmds_pack Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::return_cmds()
    {
        Cmds_pack pack;

        pack.push_back(
            make_pack_elem(
                std::string{Set_defs::set_baud_rate_s},
                Command::Policies<Numbers_only>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.debug("Received request to change baud rate...");
                        
                        if (remote_rec.conf_port == Remote_conf_port::Non_configurable)
                        {
                            set_baud_rate();
                        }
                        else
                        {
                            notyfier.error(Setter_except{"Another master on same channel, rebooting..."});;

                            notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                        }
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Get_defs::get_baud_rate_s},
                Command::Policies<Numbers_only>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.debug("Received baud rate information " + arg + "...");
                        
                        if (remote_rec.conf_port == Remote_conf_port::Non_configurable || (!check_if_all_params_established() && if_record_active()))
                        {
                            set_baud_rate_compl(arg);
                        }
                        else
                        {
                            notyfier.debug("Bad sequence...");
                            notyfier.debug("Rebooting...");

                            notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();

                            return;
                        }

                        if (if_record_active())
                        {
                            add_established_param();
                            if_all_params_established();
                        }
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Set_defs::set_parity_s},
                Command::Policies<Alpha_only>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.debug("Received request to change parity...");

                        if (remote_rec.conf_port == Remote_conf_port::Non_configurable)
                        {
                            set_parity();
                        }
                        else
                        {
                            notyfier.error(Setter_except{"Another master on same channel, rebooting..."});;

                            notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                        }
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Get_defs::get_parity_s},
                Command::Policies<Alpha_only>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.debug("Received parity information " + arg + "...");
                        
                        if (remote_rec.conf_port == Remote_conf_port::Non_configurable || (!check_if_all_params_established() && if_record_active()))
                        {
                            set_parity_compl(arg);
                        }
                        else
                        {
                            notyfier.debug("Bad sequence...");
                            notyfier.debug("Rebooting...");

                            notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();

                            return;
                        }

                        if (if_record_active())
                        {
                            add_established_param();
                            if_all_params_established();
                        }
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Set_defs::set_char_size_s},
                Command::Policies<Numbers_only>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.debug("Received request to change char size...");
                        
                        if (remote_rec.conf_port == Remote_conf_port::Non_configurable)
                        {
                            set_char_size();
                        }
                        else
                        {
                            notyfier.error(Setter_except{"Another master on same channel, rebooting..."});;

                            notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                        }
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Get_defs::get_char_size_s},
                Command::Policies<Numbers_only>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.debug("Received baud char size " + arg + "...");
                        
                        if (remote_rec.conf_port == Remote_conf_port::Non_configurable || (!check_if_all_params_established() && if_record_active()))
                        {
                            set_char_size_compl(arg);
                        }
                        else
                        {
                            notyfier.debug("Bad sequence...");
                            notyfier.debug("Rebooting...");

                            notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();

                            return;
                        }

                        if (if_record_active())
                        {
                            add_established_param();
                            if_all_params_established();
                        }
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Set_defs::set_stop_bits_s},
                Command::Policies<Alpha_only>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.debug("Received request to change stop bits...");

                        if (remote_rec.conf_port == Remote_conf_port::Non_configurable)
                        {
                            set_stop_bits();
                        }
                        else
                        {
                            notyfier.error(Setter_except{"Another master on same channel, rebooting..."});;

                            notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                        }
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Get_defs::get_stop_bits_s},
                Command::Policies<Alpha_only>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.debug("Received baud stop bits " + arg + "...");
                        
                        if (remote_rec.conf_port == Remote_conf_port::Non_configurable || (!check_if_all_params_established() && if_record_active()))
                        {
                            set_stop_bits_compl(arg);
                        }
                        else
                        {
                            notyfier.debug("Bad sequence...");
                            notyfier.debug("Rebooting...");

                            notyfier.error(Timeout_except{"Command timed out!"});

                    restart_job();
                            
                            return;
                        }

                        if (if_record_active())
                        {
                            add_established_param();
                            if_all_params_established();
                        }
                    })));

        return pack;
    }

    template <
        typename Timer_t,
        typename Remote_sett_impl,
        typename Serial_sett_impl>
    template <
        typename Device_weak_ptr_t,
        typename Remote_settings_ptr_t,
        typename Serial_settings_ptr_t>
    inline Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::Setter(
        Forwarder &&manager,
        Notyfier &&notyfier,
        Device_weak_ptr_t &&device_ptr,
        Remote_settings_ptr_t &&remote_s,
        Serial_settings_ptr_t &&serial_s,
        Remote_record &remote_rec)
        : Common_ext{
              std::move(manager),
              std::move(notyfier),
              std::forward<Device_weak_ptr_t>(device_ptr)},
          remote_s{std::forward<Remote_settings_ptr_t>(remote_s)}, serial_s{std::forward<Serial_settings_ptr_t>(serial_s)}, remote_rec{remote_rec}
    {
        assert(params_count == remote_rec.all_established && "Parameter count not match");

        add_restart_job();
        add_get_set_param_job();
        add_change_param_job();
        add_change_postponed_job();
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    template <typename... Args_t>
    inline auto Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>::make(Args_t &&...args)
    {
        return std::make_unique<Setter<Timer_t, Remote_sett_impl, Serial_sett_impl>>(std::forward<Args_t>(args)...);
    }

}