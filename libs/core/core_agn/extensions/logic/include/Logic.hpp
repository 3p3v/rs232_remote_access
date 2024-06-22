#pragma once

#include <memory>
#include <atomic>
#include <Common_ext.hpp>
#include <Mqtt_settings.hpp>
#include <Remote_status_record.hpp>
#include <Timer_cont.hpp>
/* Definitions */
#include <Hi_defs.hpp>
#include <Disconnect_defs.hpp>
/* Policies */
#include <No_arg.hpp>
/* Jobs */
#include <Restart_job.hpp>
#include <Start_job.hpp>
#include <Param_change_notify_job.hpp>
#include <Param_ready_notify_job.hpp>
#include <Get_set_param_job.hpp>
/* Except */
#include <Logic_except.hpp>
#include <Timeout_except.hpp>
#include <Disconnect_except.hpp>

namespace Logic
{
    template <
        typename Timer_t,
        typename Remote_sett_impl>
    class Logic final
        : public Common_ext<Timer_t>
    {
        /////////////////
        /* Definitions */
        /////////////////
    public:
        using Remote_settings = Mqtt_settings<Remote_sett_impl>;

    private:
        // /// @brief For setting action timeouts
        // using Timers = Timer_cont;

        ////////////////////////////////
        /* Data handled inside object */
        ////////////////////////////////
    private:
        // /// @brief Default error callback
        // /// @return
        // template <typename Str>
        // auto def_t_ec_callb(Str &&cmd_name);

        Remote_settings remote_s;

        /// @brief Remote state
        Remote_status_record &remote_rec;

        // /// @brief For setting action timeouts
        // Timers timers;

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
        void say_hi_master_keep_alive();

        /// @brief
        void say_hi_master_hi();

        //////////////////
        /* Timer resets */
        //////////////////
    private:
        // /// @brief
        // void clear_timers();
        /// @brief
        void say_hi_keep_alive_compl();
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
        Remote_ext_base::Cmds_pack return_cmds() override;

    public:
        template <
            typename Device_weak_ptr_t,
            typename Remote_settings_t>
        Logic(
            Forwarder &&manager,
            Notyfier &&notyfier,
            Device_weak_ptr_t &&device_ptr,
            Remote_settings_t &&remote_s,
            Remote_status_record &remote_rec);

        Logic(Logic &&) = delete;
        Logic &operator=(Logic &&) = delete;
        Logic(const Logic &) = delete;
        Logic &operator=(const Logic &) = delete;

        template <typename... Args_t>
        static auto make(Args_t &&...args);
    };

    template <
        typename Timer_t,
        typename Remote_sett_impl>
    template <
        typename Device_weak_ptr_t,
        typename Remote_settings_t>
    inline Logic<Timer_t, Remote_sett_impl>::Logic(
        Forwarder &&manager,
        Notyfier &&notyfier,
        Device_weak_ptr_t &&device_ptr,
        Remote_settings_t &&remote_s,
        Remote_status_record &remote_rec)
        : Common_ext{std::move(manager), std::move(notyfier), std::forward<Device_weak_ptr_t>(device_ptr)},
          remote_s{std::forward<Remote_settings_t>(remote_s)},
          remote_rec{remote_rec}
    {
        add_restart_job();
        add_start_job();
        add_param_change_notify_job();
        add_param_ready_notify_job();
    }

    template <typename Timer_t, typename Remote_sett_impl>
    template <typename... Args_t>
    inline auto Logic<Timer_t, Remote_sett_impl>::make(Args_t &&...args)
    {
        return std::make_unique<Logic<Timer_t, Remote_sett_impl>>(std::forward<Args_t>(args)...);
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::add_restart_job()
    {
        // Add job for resetting all timers (used when there was error in communication with device)
        override_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Restart_job>(
                [this](auto &job)
                {
                    notyfier.debug("Rebooting connection...");

                    /* Clear all timers */
                    timers.clear();

                    remote_rec.status = Remote_status::Disconnected;

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
                    if (remote_rec.status == Remote_status::Not_connected ||
                        remote_rec.status == Remote_status::Disconnected)
                    {
                        notyfier.debug("Starting connection...");

                        /* Say hi */
                        say_hi_();
                    }
                    else
                    {
                        throw std::logic_error{"Cannot start same serial two times!"};
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
                    if (remote_rec.status == Remote_status::Data_exchange ||
                        remote_rec.status == Remote_status::Establishing_parameters)
                    {
                        notyfier.debug("Forced changing connection parameters...");

                        /* Reestablishing connection parameters */
                        remote_rec.status = Remote_status::Establishing_parameters;
                    }
                    else
                    {
                        notyfier.debug("Cannot change connection parameters in this connection stage...");
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
                    if (remote_rec.status == Remote_status::Establishing_parameters)
                    {
                        /* Starting data exchange */
                        remote_rec.status = Remote_status::Data_exchange;
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
    inline void Logic<Timer_t, Remote_sett_impl>::reset_exts_job()
    {
        forward_job(Restart_job{});
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_()
    {
        notyfier.debug("Welcoming the device...");

        if (remote_rec.conf_port == Remote_conf_port::Configurable)
        {
            remote_s.write_s(
                Hi_defs::master_hi_s.data(),
                [serial_ctrl = shared_from_this(), this]()
                {
                    timers.start_timer(
                        Hi_defs::slave_hi_s.data(),
                        Timer_t::make_timer(
                            [serial_ctrl = shared_from_this(), this]()
                            {
                                /* Try to say hi to device again */
                                say_hi_timeout_();
                            }));
                },
                def_ec_callb());
        }
        else
        {
            remote_s.write_i(
                Hi_defs::master_keep_alive_s.data(),
                [serial_ctrl = shared_from_this(), this]()
                {
                    timers.start_timer(
                        Hi_defs::slave_keep_alive_s.data(),
                        Timer_t::make_timer(
                            [serial_ctrl = shared_from_this(), this]()
                            {
                                /* Try to say hi to device again */
                                say_hi_timeout_();
                            }));
                },
                def_ec_callb());
        }
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_timeout_()
    {
        /* Inform about error */
        notyfier.error(Timeout_except{"Saying hi timed out..."});
        
        /* Reset */
        reset_exts_job();
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline Remote_ext_base::Cmds_pack Logic<Timer_t, Remote_sett_impl>::return_cmds()
    {
        Cmds_pack pack;

        pack.push_back(
            make_pack_elem(
                std::string{Hi_defs::master_hi_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        /* No matter if configurale or not wait for response */
                        say_hi_master_hi();
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Hi_defs::slave_hi_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        /* Delete timer and get params */
                        say_hi_compl();
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Hi_defs::master_keep_alive_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        say_hi_master_keep_alive();
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Hi_defs::slave_keep_alive_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        /* Delete timer */
                        say_hi_keep_alive_compl();
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Disconnect_defs::disconne_detect_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        notyfier.error(Disconnect_except{"Server sent device disconnected!"});
                        
                        /* Reset */
                        reset_exts_job();
                    })));

        return pack;
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_master_keep_alive()
    {
        notyfier.debug("Waiting for keep alive from the device...");

        timers.start_timer(
            Hi_defs::slave_keep_alive_s.data(),
            Timer_t::make_timer(
                [serial_ctrl = shared_from_this(), this]()
                {
                    /* Try to say hi to device again */
                    say_hi_timeout_();
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_master_hi()
    {
        if (remote_rec.conf_port == Remote_conf_port::Configurable)
        {
            notyfier.error(Logic_except{"Another master operating on the same channel..."});
            notyfier.error(Logic_except{"Reseting connection..."});

            reset_exts_job();
        }
        else
        {
            notyfier.debug("Master reseted the connection...");

            timers.start_timer(
                Hi_defs::slave_hi_s.data(),
                Timer_t::make_timer(
                    [serial_ctrl = shared_from_this(), this]()
                    {
                        /* Try to say hi to device again */
                        say_hi_timeout_();
                    }));
        }
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_compl()
    {
        timers.stop_timer(Hi_defs::slave_hi_s.data());

        if (remote_rec.status == Remote_status::Not_connected || remote_rec.status == Remote_status::Disconnected)
        {
            /* Change status */
            remote_rec.status = Remote_status::Establishing_parameters;

            /* Start establishing parameters */
            if (remote_rec.conf_port == Remote_conf_port::Configurable)
            {
                notyfier.debug("Establishing connection parameters...");

                get_set_param_job();
            }
            else
            {
                notyfier.debug("Waiting for master to establish connection parameters...");
            }
        }
        else if (remote_rec.conf_port == Remote_conf_port::Configurable)
        {
            notyfier.debug("Bad sequence...");
            notyfier.debug("Rebooting...");

            restart_job();
        }
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::say_hi_keep_alive_compl()
    {
        notyfier.debug("Keep alive received...");

        timers.stop_timer(Hi_defs::slave_keep_alive_s.data());

        if (remote_rec.conf_port == Remote_conf_port::Non_configurable &&
                remote_rec.status == Remote_status::Not_connected ||
            remote_rec.status == Remote_status::Disconnected)
        {
            notyfier.debug("Reading connection parameters...");

            /* Change status */
            remote_rec.status = Remote_status::Establishing_parameters;

            /* Start establishing parameters */
            get_set_param_job();
        }
    }

    template <typename Timer_t, typename Remote_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl>::reboot()
    {
        forward_job(Restart_job{});
    }

}