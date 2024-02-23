#pragma once

#include <memory>
#include <atomic>
#include <Authed_ext.hpp>
#include <Mqtt_settings.hpp>
#include <Remote_status_record.hpp>
#include <Timer_cont.hpp>
/* Definitions */
#include <Hi_defs.hpp>
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
          protected Hi_defs,
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
    protected:
        void add_restart_job() override;

    private:
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
        /// @brief Reset all extensions
        void reset_exts_job();
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
            typename Remote_record_ptr_t,
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
                    Remote_record_ptr,
                    std::decay_t<Remote_record_ptr_t>>>>
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
        typename Remote_record_ptr_t,
        typename,
        typename,
        typename>
    inline Logic<Timer_t, Remote_sett_impl>::Logic(
        Manager_ptr_t &&manager,
        Remote_settings_ptr_t &&remote_s,
        Remote_record_ptr_t &&remote_rec)
        : Authed_ext{std::forward<Manager_ptr_t>(manager)},
          remote_s{std::forward<Remote_settings_ptr_t>(remote_s)},
          remote_rec{std::forward<Remote_record_ptr_t>(remote_rec)}
    {
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
                [this](Start_job &&job)
                {
                    if (remote_rec->status == Remote_status_record::Not_connected ||
                        remote_rec->status == Remote_status_record::Disconnected)
                    {
                        /* Set params */
                        remote_rec->port_settings = std::move(job.port_settings);

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
                    if (remote_rec->status == Remote_status::Data_exchange)
                    {
                        /* Reestablishing connection parameters */
                        remote_rec->status = Remote_status::Establishing_parameters;
                    }
                    else
                    {
                        throw std::logic_error{"Bad job order!"};
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
    inline void Logic<Timer_t, Remote_sett_impl>::reset_exts_job()
    {
        forward_job(Restart_job{});
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::get_set_param_job()
    {
        forward_job(Get_set_param_job{});
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_()
    {
        if (remote_rec->conf_port)
        {
            remote_s.write_s(
                master_hi_s.data(),
                [serial_ctrl = shared_from_this(), this]()
                {
                    say_hi();
                },
                def_ec_callb());
        }
        else
        {
            remote_s.write_s(
                master_keep_alive_s.data(),
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
        say_hi_();
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline Remote_ext_base::Cmds_pack Logic<Timer_t, Remote_sett_impl>::get_cmds()
    {
        Cmds_pack pack;

        pack.emplace(
            std::string{master_hi_s},
            Command::Policies<No_arg>::Dyn_handle(
                [](const std::string &arg)
                { say_hi_(); }));

        pack.emplace(
            std::string{slave_hi_s},
            Command::Policies<No_arg>::Dyn_handle(
                [](const std::string &arg)
                {
                    /* Delete timer */
                    helper.say_hi_compl();

                    /* Start establishing parameters */
                    get_set_param_job();
                }));

        return pack;
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi()
    {
        if (remote_rec->conf_port)
        {
            timers.start_timer(
                slave_hi_s.data(),
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
                slave_keep_alive_s.data(),
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
        timers.stop_timer(master_hi_s.data());
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::keep_alive_compl()
    {
        timers.stop_timer(master_keep_alive_s.data());
    }
}