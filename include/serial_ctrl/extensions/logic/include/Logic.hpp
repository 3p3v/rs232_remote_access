#pragma once

#include <memory>
#include <atomic>
#include <Authed_ext.hpp>
#include <Mqtt_settings.hpp>
#include <Remote_status_record.hpp>
#include <Timer_cont.hpp>
/* Definitions */
#include <Hi_defs.hpp>
/* Policies */
#include <No_arg.hpp>
/* Jobs */
#include <Restart_job.hpp>
#include <Start_job.hpp>
#include <Param_change_notify_job.hpp>
#include <Param_ready_notify_job.hpp>
#include <Get_set_param_job.hpp>

namespace Logic
{
    template <
        typename Timer_t,
        typename Remote_sett_impl>
    class Logic final
        : public Authed_ext,
          public std::enable_shared_from_this<Logic<Timer_t, Remote_sett_impl>>
    {
        /////////////////
        /* Definitions */
        /////////////////
    public:
        using Remote_settings_ptr = Mqtt_settings<Remote_sett_impl>;
        using Remote_record_ptr = std::shared_ptr<Remote_status_record>;

    private:
        /// @brief For setting action timeouts
        using Timers = Timer_cont;

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
        Remote_record_ptr remote_rec;

        /// @brief For setting action timeouts
        Timers timers;

        /////////////////
        /* Jobs to add */
        /////////////////
    private:
        void add_restart_job() override;
        /// @brief Send start command
        void add_start_job();
        /// @brief
        void add_param_change_notify_job();
        /// @brief
        void add_param_ready_notify_job();

        ////////////////////////////////
        /* Actions with other modules */
        ////////////////////////////////
    private:
        /// @brief
        void get_set_param_job();

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

        //////////////////
        /* Timer resets */
        //////////////////
    private:
        /// @brief
        void clear_timers();
        /// @brief
        void say_hi_compl();
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
            typename Remote_record_ptr_t>//,
            // typename = std::enable_if_t<
            //     std::is_base_of_v<
            //         Manager_ptr,
            //         std::decay_t<Manager_ptr_t>>>,
            // typename = std::enable_if_t<
            //     std::is_base_of_v<
            //         Remote_settings_ptr,
            //         std::decay_t<Remote_settings_ptr_t>>>,
            // typename = std::enable_if_t<
            //     std::is_base_of_v<
            //         Remote_record_ptr,
            //         std::decay_t<Remote_record_ptr_t>>>>
        Logic(
            Manager_ptr_t &&manager,
            Remote_settings_ptr_t &&remote_s,
            Remote_record_ptr_t &&remote_rec);
    };

    template <
        typename Timer_t,
        typename Remote_sett_impl>
    template <
        typename Manager_ptr_t,
        typename Remote_settings_ptr_t,
        typename Remote_record_ptr_t>//,
        // typename,
        // typename,
        // typename>
    inline Logic<Timer_t, Remote_sett_impl>::Logic(
        Manager_ptr_t &&manager,
        Remote_settings_ptr_t &&remote_s,
        Remote_record_ptr_t &&remote_rec)
        : Authed_ext{std::forward<Manager_ptr_t>(manager)},
          remote_s{std::forward<Remote_settings_ptr_t>(remote_s)},
          remote_rec{std::forward<Remote_record_ptr_t>(remote_rec)}
    {
        add_restart_job();
        add_start_job();
        add_param_change_notify_job();
        add_param_ready_notify_job();
    }

    template <typename Timer_t, typename Remote_sett_impl>
    template <typename Str>
    inline auto Logic<Timer_t, Remote_sett_impl>::def_t_ec_callb(Str &&cmd_name)
    {
        return [cmd_name = std::forward<Str>(cmd_name)]() mutable
        {
            /* Tell monitor about timeout */
        };
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::add_restart_job()
    {
        // Add job for resetting all timers (used when there was error in communication with device)
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Restart_job>(
                [this](auto &job)
                {
                    /* Clear all timers */
                    timers.clear();

                    remote_rec->status = Remote_status::Disconnected;

                    /* Say hi */
                    say_hi_();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::add_start_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Start_job>(
                [this](auto &&job)
                {
                    if (remote_rec->status == Remote_status::Not_connected ||
                        remote_rec->status == Remote_status::Disconnected)
                    {
                        /* Say hi */
                        say_hi_();
                    }
                    else
                    {
                        throw std::logic_error{"Can not start same serial two times!"};
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
                    if (remote_rec->status == Remote_status::Data_exchange ||
                        remote_rec->status == Remote_status::Establishing_parameters)
                    {
                        /* Reestablishing connection parameters */
                        remote_rec->status = Remote_status::Establishing_parameters;
                    }
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
                    if (remote_rec->status == Remote_status::Establishing_parameters)
                    {
                        /* Starting data exchange */
                        remote_rec->status = Remote_status::Data_exchange;
                    }
                    else
                    {
                        throw std::logic_error{"Bad job order!"};
                    }
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::get_set_param_job()
    {
        forward_job(Get_set_param_job{});
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_()
    {
        if (remote_rec->conf_port == Remote_conf_port::Configurable)
        {
            remote_s.write_s(
                Hi_defs::master_hi_s.data(),
                [serial_ctrl = shared_from_this(), this]()
                {
                    say_hi();
                },
                def_ec_callb());
        }
        else
        {
            remote_s.write_i(
                Hi_defs::master_keep_alive_s.data(),
                [serial_ctrl = shared_from_this(), this]()
                {
                    say_hi();
                },
                def_ec_callb());
        }
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_timeout_()
    {
        reset_exts_job();
        remote_rec->status = Remote_status::Disconnected;
        say_hi_();
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline Remote_ext_base::Cmds_pack Logic<Timer_t, Remote_sett_impl>::get_cmds()
    {
        Cmds_pack pack;

        pack.push_back(
            make_pack_elem(
                std::string{Hi_defs::master_hi_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        say_hi();
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Hi_defs::slave_hi_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        /* Delete timer */
                        say_hi_compl();

                        /* Change status */
                        remote_rec->status = Remote_status::Establishing_parameters;

                        /* Start establishing parameters */
                        get_set_param_job();
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Hi_defs::master_keep_alive_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        say_hi_();
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Hi_defs::slave_keep_alive_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        /* Delete timer */
                        say_hi_compl();

                        /* Change status */
                        remote_rec->status = Remote_status::Establishing_parameters;

                        /* Start establishing parameters */
                        get_set_param_job();
                    })));

        return pack;
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi()
    {
        if (remote_rec->conf_port == Remote_conf_port::Configurable)
        {
            timers.start_timer(
                Hi_defs::slave_hi_s.data(),
                Timer_t::make_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    }));
        }
        else
        {
            timers.start_timer(
                Hi_defs::slave_keep_alive_s.data(),
                Timer_t::make_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    }));
        }
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::clear_timers()
    {
        timers.clear();
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_compl()
    {
        if (remote_rec->conf_port == Remote_conf_port::Configurable)
        {
            timers.stop_timer(Hi_defs::master_hi_s.data());
        }
        else
        {
            timers.stop_timer(Hi_defs::slave_keep_alive_s.data());
        }
    }
}