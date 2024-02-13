#pragma once

#include <memory>
#include <atomic>
#include <Authed_ext.hpp>
#include <Mqtt_settings.hpp>
#include <Remote_record.hpp>
/* Definitions */
#include <Hi_defs.hpp>
/* Jobs */
#include <Restart_job.hpp>

namespace Logic
{
    template <
        typename Timer_t,
        typename Remote_sett_impl,
        typename Serial_sett_impl>
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
        using Serial_settings_ptr = std::shared_ptr<Serial_settings<Serial_sett_impl>>;
        using Remote_record_ptr = std::shared_ptr<Remote_record>;
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
        auto def_ec_callb();

        /// @brief Default error callback
        /// @return
        template <typename Str>
        auto def_t_ec_callb(Str &&cmd_name);

        Remote_settings_ptr remote_s;
        Serial_settings_ptr serial_s;
        /// @brief Port settings, remote state
        Remote_record_ptr remote_rec;

        /// @brief For setting action timeouts
        Timers timers;

        /////////////////
        /* Jobs to add */
        /////////////////
    protected:
        void add_restart_job() override;

    private:
        /// @brief Configurable device?
        bool conf_port{true};
        /// @brief Connect to remote and serial
        void add_start_job();

        ////////////////////////////////
        /* Actions with other modules */
        ////////////////////////////////
    private:
        /// @brief Reset all extensions
        void reset_exts();

        ////////////////////////////////////////////
        /* Commands sent to info and set channels */
        ////////////////////////////////////////////
    private:
        /// @brief
        void say_hi();
        /// @brief Get current settings
        void get_settings();

        
        //////////////////////////////////////////////////
        /* Commands received from info and set channels */
        //////////////////////////////////////////////////
    private:
        /* Wait for params */
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
        void clear_timers();
        /// @brief
        void say_hi_compl();
        /// @brief
        void reboot();

        /* Set param completer */
        /// @brief
        void set_baud_rate_compl(const std::string &arg);
        /// @brief
        void set_parity_compl(const std::string &arg);
        /// @brief
        void set_char_size_compl(const std::string &arg);
        /// @brief
        void set_stop_bits_compl(const std::string &arg);

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
                    std::decay_t<Serial_settings_ptr_t>>>,
            typename = std::enable_if_t<
                std::is_same_v<
                    Remote_record_ptr,
                    std::decay_t<Remote_record_ptr_t>>>>
        Logic(
            Manager_ptr_t &&manager,
            Remote_settings_ptr_t<Remote_sett_impl> &&remote_s,
            Serial_settings_ptr_t<Serial_sett_impl> &&serial_s,
            Remote_record_ptr_t &&remote_rec);
    };

    template <
        typename Timer_t,
        typename Remote_sett,
        typename Serial_sett>
    template <
        typename Manager_ptr_t,
        typename Remote_settings_ptr_t,
        typename Serial_settings_ptr_t,
        typename Remote_record_ptr_t,
        typename,
        typename,
        typename,
        typename>
    inline Logic::Logic(
        Manager_ptr_t &&manager,
        Remote_settings_ptr_t &&remote_s,
        Serial_settings_ptr_t &&serial_s,
        Remote_record_ptr_t &&remote_rec)
        : Authed_ext{std::forward<Manager_ptr_t>(manager)},
          remote_s{std::forward<Remote_settings_ptr_t>(remote_s)},
          serial_s{std::forward<Serial_settings_ptr_t>(serial_s)},
          remote_rec{std::forward<Remote_record_ptr_t>(remote_rec)}
    {
        add_start_job();
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline auto Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::def_ec_callb()
    {
        return [](const std::except &e) {};
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
                [this]()
                {
                    /* Clear all timers */
                    timers.clear();

                    /* Say hi */
                    remote_s.write_s(master_hi_s.data());
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
                    if (remote_rec->status == Remote_status::not_connected ||
                        remote_rec->status == Remote_status::disconnected)
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

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline void Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::reset_exts()
    {
        manager->forward_job(Restart_job{});
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Serial_sett_impl>
    inline Cmds_pack Logic<Timer_t, Remote_sett_impl, Serial_sett_impl>::get_cmds()
    {
        Cmds_pack pack;

        pack.emplace(
            std::string{master_hi_s},
            Command::Policies<No_arg>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                     const std::string &arg)
                                                  { helper.say_hi(); }));

        pack.emplace(
            std::string{slave_hi_s},
            Command::Policies<No_arg>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                     const std::string &arg)
                                                  { 
                                                                /* Delete timer */
                                                                helper.say_hi_compl();

                                                                /* Start keep alive timer */
                                                                helper.keep_alive_start();

                                                                /* Get settings if unknown */
                                                                if (!helper.get_helper().settings_known)
                                                                {
                                                                        helper.get_settings();
                                                                }
                                                                /* Else send settings */
                                                                else
                                                                {
                                                                        helper.set_baud_rate(helper.get_info().baud_rate);
                                                                        helper.set_parity(helper.get_info().parity);
                                                                        helper.set_char_size(helper.get_info().char_size);
                                                                        helper.set_stop_bits(helper.get_info().stop_bits);
                                                                } }));

        return pack;
    }
}