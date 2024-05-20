#pragma once

#include <Common_ext.hpp>
#include <Mqtt_settings.hpp>
#include <Mqtt_side.hpp>
#include <Serial_side.hpp>
/* Message count */
#include <Packet_defs.hpp>
#include <Packet_master.hpp>
#include <Mqtt_msg_cont.hpp>
#include <Packet_slave.hpp>
#include <Packet_controller.hpp>
/* Jobs */
#include <Param_change_notify_job.hpp>
#include <Param_ready_notify_job.hpp>
/* Policies */
#include <No_arg.hpp>
/**/
#include <Remote_conf_port_rec.hpp>
/**/
#include <atomic>

namespace Logic
{
    template <
        typename Timer_t,
        typename Remote_sett_impl,
        typename Remote_side_impl,
        typename Serial_side_impl>
    class Exchanger
        : public Common_ext<Timer_t>
    {
        /////////////////
        /* Definitions */
        /////////////////
    public:
        using Remote_settings_ptr = Mqtt_settings<Remote_sett_impl>;
        using Remote_side_ptr = Mqtt_side<Remote_side_impl>;
        using Serial_side_ptr = Serial_side<Serial_side_impl>;

    private:
        using Msg_num_type = std::decay_t<decltype(Packet_defs::max_msg_num)>;
        using Packet_m = Packet_controller<Msg_num_type, Packet_defs::min_msg_num, Packet_defs::max_msg_num, 15>;
        using Packet_s = Packet_slave<Msg_num_type, Packet_defs::min_msg_num, Packet_defs::max_msg_num>;

        ////////////////////////////////
        /* Data handled inside object */
        ////////////////////////////////
    private:
        /// @brief Remote settings
        Remote_settings_ptr remote_s;
        /// @brief Remote data
        Remote_side_ptr remote_d;
        /// @brief Local data
        Serial_side_ptr serial_d;

        /// @brief Port settings, remote state
        Remote_conf_port_rec &remote_rec;

        /// @brief Message counter for master
        Packet_m count_m{};
        /// @brief
        Packet_s count_s{};

        std::atomic_bool comm_unlock{false};

        /// @brief Is communication to serial allowed?
        /// @return
        bool remote_to_serial();
        /// @brief Is communication to remote allowed?
        /// @return
        bool serial_to_remote();

        /////////////////
        /* Jobs to add */
        /////////////////
    protected:
        /// @brief
        void add_restart_job() override;

    private:
        /// @brief Stop communication
        void add_param_change_notify_job();
        /// @brief Start communication
        void add_param_ready_notify_job();

        ////////////////////////////////////////////
        /* Commands sent to info and set channels */
        ////////////////////////////////////////////
    private:
        /// @brief Ask for packet from remote
        void ask_to_resend(Msg_num_type id);
        /// @brief Send ack
        void send_ack(Msg_num_type id);

        //////////////////////////////////////////////////
        /* Commands received from info and set channels */
        //////////////////////////////////////////////////
        /* Timer sets */
        ////////////////
    private:
        /// @brief Resend packet if remote asks for it
        void resend(Msg_num_type id);
        /// @brief Delete messages after receiving ack
        void ack(Msg_num_type id);

        //////////////
        /* External */
        //////////////
    public:
        /// @brief Get all commands that the extension is handling
        /// @return
        Unauthed_ext::Cmds_pack get_cmds() override;

        auto get_remote_args();
        auto get_serial_args();

        template <
            typename Device_weak_ptr_t,
            typename Remote_settings_ptr_t,
            typename Remote_side_ptr_t,
            typename Serial_side_ptr_t>
        Exchanger(
            Forwarder &&manager,
            Notyfier &&notyfier,
            Device_weak_ptr_t &&device_ptr,
            Remote_settings_ptr_t &&remote_s,
            Remote_side_ptr_t &&remote_d,
            Serial_side_ptr_t &&serial_d,
            Remote_conf_port_rec &remote_rec);
        Exchanger(Exchanger &&) noexcept;
        Exchanger &operator=(Exchanger &&) noexcept;
        Exchanger(const Exchanger &) = delete;
        Exchanger &operator=(const Exchanger &) = delete;
        ~Exchanger() = default;
    };

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline bool Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::remote_to_serial()
    {
        return comm_unlock.load();
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline bool Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::serial_to_remote()
    {
        return remote_rec.conf_port == Remote_conf_port::Configurable && comm_unlock.load();
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline void Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::add_restart_job()
    {
        // Add job for resetting all timers (used when there was error in communication with device)
        override_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Restart_job>(
                [this](auto &job)
                {
                    /* Clear all timers */
                    timers.clear();

                    /* Reset packet nums*/
                    count_s.reload();
                    count_m.reload();

                    /* Lock ports and wait for connection to be reestablished */
                    comm_unlock = false;
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline void Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::add_param_change_notify_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Param_change_notify_job>(
                [this](auto &job)
                {
                    /* Reestablishing connection parameters */
                    comm_unlock = false;
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline void Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::add_param_ready_notify_job()
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Param_ready_notify_job>(
                [this](auto &job)
                {
                    /* Ready to exchange data */
                    comm_unlock = true;
                }));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline void Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::ack(Msg_num_type id)
    {
        count_m.ack(id);
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline void Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::send_ack(Msg_num_type id)
    {
        count_s.ack(id);
        remote_s.write_i(
            Packet_defs::packet_ack_s.data(), id, []() {}, []() {});
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline void Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::ask_to_resend(Msg_num_type id)
    {
        remote_s.write_i(
            Packet_defs::invalid_number_s.data(), id, []() {}, []() {});
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline void Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::resend(Msg_num_type id)
    {
        auto msgs_ids = count_m.get(id);

        std::for_each(
            msgs_ids.begin(),
            msgs_ids.end(),
            [this](auto &id)
            {
                auto &msg = count_m[id];

                /* Send MQTT message */
                remote_d.write(
                    id,
                    msg.begin(),
                    msg.begin() + msg.data_len,
                    [ptr = shared_from_this(),
                     this,
                     id](char, size_t)
                    {
                        /* Mark message as unused */
                        count_m[id].unused();
                    },
                    [](const std::exception &e) {

                    });
            });
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline Unauthed_ext::Cmds_pack Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::get_cmds()
    {
        Cmds_pack pack;

        pack.push_back(
            make_pack_elem(
                std::string{Packet_defs::packet_ack_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        ack(arg[0]);
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Packet_defs::invalid_number_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        resend(arg[0]);
                    })));

        pack.push_back(
            make_pack_elem(
                std::string{Packet_defs::no_number_s},
                Command::Policies<No_arg>::Dyn_handle(
                    [this](const std::string &arg)
                    {
                        // TODO send info to monitor

                        restart_job();
                    })));

        return pack;
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline auto Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::get_remote_args()
    {
        auto ok_callb = [ptr = weak_from_this(), this](auto num, auto begin, auto end, auto callb)
        {
            if (auto p = ptr.lock() && remote_to_serial())
            {
                try
                {
                    /* Check number */
                    count_s.num_up(num);

                    if (count_s.get_not_acked() > Packet_defs::ack_after)
                    {
                        send_ack(count_s.exp());
                    }
                }
                catch (const std::logic_error &e)
                {
                    /* Ask for packet with expected number */
                    ask_to_resend(count_s.exp());
                    return;
                }

                try
                {
                    /* Write to serial, save callback */
                    serial_d.write(
                        begin,
                        end,
                        [callb = std::forward<decltype(callb)>(callb)](size_t)
                        {
                            /* Run callback */
                            callb();
                        },
                        [](const std::exception &e) {

                        });
                }
                catch (const std::exception &e)
                {
                }
            }
        };

        auto ec_callb = [ptr = weak_from_this()](const std::exception &e)
        {
            if (auto p = ptr.lock())
            {
                // TODO notify
            }
        };

        return std::make_tuple(std::move(ok_callb), std::move(ec_callb));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline auto Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::get_serial_args()
    {
        /* Get iterators for new message */
        auto &msg = count_m.create([](const std::logic_error &e) {

        });
        auto msg_begin = msg.begin();
        auto msg_end = msg.end();
        auto msg_id = msg.id();

        // auto ok_callb = [ptr = shared_from_this(), this, id = msg_id](auto begin, auto end, auto callb) mutable
        auto ok_callb = [ptr = weak_from_this(), this, id = msg_id](auto msg_begin, auto msg_end, auto &&callb) mutable
        {
            if (auto p = ptr.lock() && serial_to_remote())
            {
                try
                {
                    /* Retrive buffers of newly created message */
                    auto old_id = id;
                    auto &old_msg = count_m[id];
                    /* Set message length */
                    old_msg.data_len = msg_end - msg_begin;

                    /* Create new message, save id */
                    id = count_m.create(
                                    [](const std::logic_error &e) {

                                    })
                             .id();

                    /* Send MQTT message */
                    remote_d.write(
                        old_id,
                        msg_begin,
                        msg_end,
                        [ptr = shared_from_this(),
                         this,
                         id,
                         old_id,
                         callb = std::forward<decltype(callb)>(callb)](char, size_t)
                        {
                            /* Mark last message as unused */
                            count_m[old_id].unused();

                            /* Retrive next messages buffers */
                            auto &msg = count_m[id];

                            /* Set new buffer */
                            callb(msg.begin(), msg.end());

                            /* Check if messages were ACKed */
                            if (count_m.get_not_acked() > Packet_defs::max_not_ack)
                            {
                                // ERROR
                            }
                        },
                        [](const std::exception &e) {

                        });
                }
                catch (const std::logic_error &e)
                {
                    if (auto p = ptr.lock())
                    {
                        // TODO notify
                    }
                }
            }
        };

        auto ec_callb = [ptr = weak_from_this()](const std::exception &e) {

        };

        return std::make_tuple(msg_begin, msg_end, std::move(ok_callb), std::move(ec_callb));
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::Exchanger(Exchanger &&e) noexcept
        : Common_ext{std::move(e)},
          remote_s{std::move(e.remote_s)},
          remote_d{std::move(e.remote_d)},
          serial_d{std::move(e.serial_d)},
          remote_rec{e.remote_rec},
          count_m{std::move(e.count_m)},
          count_s{std::move(e.count_s)},
          comm_unlock{e.comm_unlock.load()}
    {
    }

    template <typename Timer_t, typename Remote_sett_impl, typename Remote_side_impl, typename Serial_side_impl>
    inline Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl> &
    Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::operator=(Exchanger &&e) noexcept
        : Exchanger{std::move(e)}
    {
        // TODO: insert return statement here
    }

    template <
        typename Timer_t,
        typename Remote_sett_impl,
        typename Remote_side_impl,
        typename Serial_side_impl>
    template <
        typename Device_weak_ptr_t,
        typename Remote_settings_ptr_t,
        typename Remote_side_ptr_t,
        typename Serial_side_ptr_t>
    inline Exchanger<Timer_t, Remote_sett_impl, Remote_side_impl, Serial_side_impl>::Exchanger(
        Forwarder &&manager,
        Notyfier &&notyfier,
        Device_weak_ptr_t &&device_ptr,
        Remote_settings_ptr_t &&remote_s,
        Remote_side_ptr_t &&remote_d,
        Serial_side_ptr_t &&serial_d,
        Remote_conf_port_rec &remote_rec)
        : Common_ext{std::move(manager), std::move(notyfier), std::forward<Device_weak_ptr_t>(device_ptr)},
          remote_s{std::forward<Remote_settings_ptr_t>(remote_s)},
          remote_d{std::forward<Remote_side_ptr_t>(remote_d)},
          serial_d{std::forward<Serial_side_ptr_t>(serial_d)},
          remote_rec{remote_rec}
    {
        add_restart_job();
        add_param_change_notify_job();
        add_param_ready_notify_job();
    }
}