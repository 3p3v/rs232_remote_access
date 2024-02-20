#pragma once

#include <memory>
#include <atomic>
#include <Authed_ext.hpp>
#include <Mqtt_settings.hpp>
#include <Remote_status_record.hpp>
/* Definitions */
#include <Hi_defs.hpp>
/* Jobs */
#include <Restart_job.hpp>
#include <Start_job.hpp>
#include <Change_param_job.hpp>
#include <Param_ready_notify_job.hpp>

namespace Logic
{
    template <
        typename Timer_t,
        typename Remote_sett_impl>
    class Logic final
        : public Authed_ext,
          protected Hi_defs,
          public std::enable_shared_from_this
    {
        /////////////////
        /* Definitions */
        /////////////////
    public:
        using Remote_settings_ptr = std::shared_ptr<Mqtt_settings<Remote_sett_impl>>;
        using Remote_record_ptr = std::shared_ptr<Remote_record>;
        using Remote_status_record_ptr = std::shared_ptr<Remote_status_record>;
        using Next_cmds = std::vector<std::string_view &>;
        using Next_cmds_ptr = std::shared_ptr<Next_cmds>;

    private:
        /// @brief For setting action timeouts
        using Timers = Timer_cont<Timer_t>;

        ////////////////////////////////
        /* Data handled inside object */
        ////////////////////////////////
    private:
        /// @brief Default error callback
        /// @return
        template <typename Str>
        auto def_t_ec_callb(Str &&cmd_name);

        Remote_settings_ptr remote_s;
        
        /// @brief Remote state
        Remote_status_record_ptr remote_rec;

        /// @brief For setting action timeouts
        Timers timers;

        /////////////////
        /* Jobs to add */
        /////////////////
    protected:
        void add_restart_job() override;

    private:
        /// @brief Connect to remote and serial
        void add_start_job();
        /// @brief 
        void add_param_change_notify_job();
        /// @brief 
        void add_param_ready_notify_job();

        ////////////////////////////////
        /* Actions with other modules */
        ////////////////////////////////
    private:
        /// @brief Reset all extensions
        void reset_exts_job();

        ////////////////////////////////////////////
        /* Commands sent to info and set channels */
        ////////////////////////////////////////////
    private:
        /// @brief
        void say_hi_();
        /// @brief
        void say_hi_timeout_();

        //////////////////////////////////////////////////
        /* Commands received from info and set channels */
        //////////////////////////////////////////////////
        /* Timer sets */
        ////////////////
    private:
        /// @brief
        void say_hi();
        /// @brief 
        void keep_alive();

        //////////////////
        /* Timer resets */
        //////////////////
    private:
        /// @brief
        void clear_timers();
        /// @brief
        void say_hi_compl();
        /// @brief 
        void keep_alive_compl();
        /// @brief
        void reboot();

        //////////////
        /* External */
        //////////////
    public:
        /// @brief Get all commands that the extension is handling
        /// @return
        Cmds_pack get_cmds() override;

        template <
            typename Manager_ptr_t,
            typename Remote_settings_ptr_t,
            typename Serial_settings_ptr_t,
            template Remote_record_ptr_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Manager_ptr,
                    std::decay_t<Manager_ptr_t>>>,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Remote_settings_ptr,
                    std::decay_t<Remote_settings_ptr_t>>>,
            typename = std::enable_if_t<
                std::is_same_v<
                    Serial_settings_ptr,
                    std::decay_t<Serial_settings_ptr_t>>>>
        Logic(
            Manager_ptr_t &&manager,
            Remote_settings_ptr_t<Remote_sett_impl> &&remote_s,
            Serial_settings_ptr_t<Serial_sett_impl> &&serial_s);
    };

    template <
        typename Timer_t,
        typename Remote_sett_impl>
    template <
        typename Manager_ptr_t,
        typename Remote_settings_ptr_t,
        typename Serial_settings_ptr_t,
        typename,
        typename,
        typename>
    inline Logic::Logic(
        Manager_ptr_t &&manager,
        Remote_settings_ptr_t &&remote_s,
        Serial_settings_ptr_t &&serial_s)
        : Authed_ext{std::forward<Manager_ptr_t>(manager)},
          remote_s{std::forward<Remote_settings_ptr_t>(remote_s)},
          serial_s{std::forward<Serial_settings_ptr_t>(serial_s)}
    {
        add_start_job();
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    template <typename Str>
    inline auto Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::def_t_ec_callb(Str &&cmd_name)
    {
        return [cmd_name = std::forward<Str>(cmd_name)]() mutable
        {
            /* Tell monitor about timeout */
        };
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_restart_job()
    {
        // Add job for resetting all timers (used when there was error in communication with device)
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Restart_job>(
                [this](auto &job)
                {
                    /* Clear all timers */
                    timers.clear();

                    /* Say hi */
                    say_hi_();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::add_start_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Start_job>(
                [this](Start_job &&job)
                {
                    if (remote_rec->status == Remote_status_record::not_connected ||
                        remote_rec->status == Remote_status_record::disconnected)
                    {
                        /* Set params */
                        remote_rec->port_settings = std::move(job.port_settings);

                        /* Say hi */
                        remote_s.write_s(
                            master_hi_s.data(),
                            [this, logic_ = std::shared_from_this()]() mutable
                            {
                                timers.start_timer(
                                    master_hi_s.data(),
                                    def_t_ec_callb(master_hi_s.data()));
                            },
                            def_ec_callb());
                    }
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::add_param_change_notify_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Param_change_notify_job>(
                [this](auto &job)
                {
                    /* Reestablishing connection parameters */
                    remote_rec->status = Remote_status::establishing_parameters;
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::add_param_ready_notify_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Param_ready_notify_job>(
                [this](auto &job)
                {
                    /* Reestablishing connection parameters */
                    remote_rec->status = Remote_status::data_exchange;
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::reset_exts_job()
    {
        manager->forward_job(Restart_job{});
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::say_hi_()
    {
        if (remote_rec->conf_port)
        {
            remote_s.write_s(
                std::string{master_hi_s},
                [serial_ctrl = shared_from_this(), this]()
                {
                    say_hi_timeout_();
                },
                def_ec_callb());
        }
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::say_hi_timeout_()
    {
        reset_exts_job();
        
        if (remote_rec->conf_port)
        {
            remote_s.write_s(
                std::string{master_hi_s},
                [serial_ctrl = shared_from_this(), this]()
                {
                    // TODO tell monitor about timeout
                    say_hi_timeout_();
                },
                def_ec_callb());
        }
        else
        {
            remote_s.write_i(
                std::string{master_keep_alive_s},
                [serial_ctrl = shared_from_this(), this]()
                {
                    // TODO tell monitor about timeout
                    say_hi_timeout_();
                },
                def_ec_callb());
        }
    }

    

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline Cmds_pack Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_cmds()
    {
        Cmds_pack pack;

        pack.emplace(
            std::string{master_hi_s},
            Command::Policies<No_arg>::Dyn_handle([](const std::string &arg)
                                                  { say_hi_(); }));

        pack.emplace(
            std::string{slave_hi_s},
            Command::Policies<No_arg>::Dyn_handle([](const std::string &arg)
                                                  { 
                                                    /* Delete timer */
                                                    helper.say_hi_compl();

                                                    /* Get settings if unknown or not configurable */
                                                    if (!remote_rec->settings_known || !remote_rec->conf_port)
                                                    {
                                                        get_settings_();
                                                    }
                                                    /* Else send settings */
                                                    else
                                                    {
                                                        set_baud_rate(remote_rec->port_settings.baud_rate);
                                                        set_parity(remote_rec->port_settings.parity);
                                                        set_char_size(remote_rec->port_settings.char_size);
                                                        set_stop_bits(remote_rec->port_settings.stop_bits);
                                                    } }));

        return pack;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::say_hi()
    {
        timers.start_timer(
            std::string{master_hi_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::keep_alive()
    {
        timers.start_timer(
            std::string{keep_alive_s},
            timers.start_timer(
                Custom_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    });));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::clear_timers()
    {
        timers.clear();
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::keep_alive_compl()
    {
        timers.stop_timer(std::string{keep_alive_s});
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::say_hi_timeout_compl()
    {
        timers.stop_timer(std::string{master_hi_s});
    }
}