#pragma once

#include <Proto_module.hpp>
/* Message count */
#include <Packet_defs.hpp>
#include <Packet_master.hpp>
#include <Mqtt_msg_cont.hpp>
#include <Packet_slave.hpp>
#include <Packet_controller.hpp>
#include <Packet_sett_final.hpp>

namespace Logic
{
    template <
        typename Device_t>
    class Exchanger final : public Proto_module
    {
        using Msg_num_type = Packet_defs::Val_t;
        using Packet_m = Packet_controller<Msg_num_type>;
        using Packet_s = Packet_slave<Msg_num_type>;

    private:
        /// @brief Message counter for master
        Packet_m count_m{Packet_sett_final::get()};
        /// @brief Message counter for slave
        Packet_s count_s{Packet_sett_final::get()};
        /// @brief Temporary variable
        Msg_num_type msg_id;

        bool comm_unlock{false};

        /// @brief Is communication to serial allowed?
        /// @return
        bool remote_to_serial();
        /// @brief Is communication to remote allowed?
        /// @return
        bool serial_to_remote();

    private:
        /// @brief Create callbacks for data channel from remote
        /// @return 
        auto get_remote_args();
    
        auto get_serial_args_helper_0();

        template <typename Mqtt_msg_t>
        auto get_serial_args_helper_1(Mqtt_msg_t &msg);

        /// @brief Create callbacks for data channel from serial
        /// @return 
        auto get_serial_args();

    public:
        /// @brief Ask for packet from remote
        void ask_to_resend(Msg_num_type id);
        /// @brief Send ack
        void send_ack(Msg_num_type id);

        /// @brief Resend packet if remote asks for it
        void resend(Msg_num_type id);
        /// @brief Delete messages after receiving ack
        void ack(Msg_num_type id);

        /// @brief Restart data communication
        void restart() override;
    };

    template <typename Device_t>
    inline bool Exchanger<Device_t>::remote_to_serial()
    {
        return comm_unlock.load();
    }

    template <typename Device_t>
    inline bool Exchanger<Device_t>::serial_to_remote()
    {
        return remote_rec.conf_port == Remote_conf_port::Configurable && comm_unlock.load();
    }

    template <typename Device_t>
    inline void Exchanger<Device_t>::ack(Msg_num_type id)
    {
        count_m.ack(id);
    }

    template <typename Device_t>
    inline void Exchanger<Device_t>::send_ack(Msg_num_type id)
    {
        count_s.ack(id);
        remote_s.write_i(
            Packet_defs::packet_ack_s.data(), id,
            []() {},
            [ptr = shared_from_this(), this](const auto &e)
            {
                device.notifier.error(e);
            });
    }

    template <typename Device_t>
    inline void Exchanger<Device_t>::ask_to_resend(Msg_num_type id)
    {
        remote_s.write_i(
            Packet_defs::invalid_number_s.data(), id,
            []() {},
            [ptr = shared_from_this(), this](const auto &e)
            {
                device.notifier.error(e);
            });
    }

    template <typename Device_t>
    inline void Exchanger<Device_t>::resend(Msg_num_type id)
    {
        auto msgs_ids = count_m.get(id);

        if (msgs_ids.size() == 0)
        {
            device.notifier.error(Data_loss_except{"Cannot resend requested packet!"});
        }
        else
        {
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
                        msg.begin() + msg.get_len(),
                        [ptr = shared_from_this(),
                        this,
                        id](char, size_t)
                        {
                            /* Mark message as unused */
                            count_m[id].unused();
                        },
                        [ptr = shared_from_this(), this](const auto &e)
                        {
                            device.notifier.error(e);
                        });
                });
        }
    }


    template <typename Device_t>
    inline auto Exchanger<Device_t>::get_remote_args()
    {
        auto ok_callb = [ptr = weak_from_this(), this](auto num, auto begin, auto end, auto callb)
        {
            if (auto p = ptr.lock() && remote_to_serial())
            {
                try
                {
                    Unique_guard lock{count_mutex};
                    
                    /* Check number */
                    count_s.num_up(num);

                    if (count_s.get_not_acked() > Packet_defs::ack_after)
                    {
                        send_ack(count_s.exp());
                    }

                    lock.unlock();

                    /* Write to serial, save callback */
                    serial_d.write(
                        begin,
                        end,
                        [ptr = std::move(p), callb = std::forward<decltype(callb)>(callb)](size_t)
                        {
                            /* Run callback */
                            callb();
                        },
                        [ptr = shared_from_this(), this](const auto &e)
                        {
                            device.notifier.error(e);
                        });
                }
                catch (const std::logic_error &)
                {
                    device.notifier.debug("Received wrong packet number: " + std::to_string(num) + "...");

                    

                    /* Ask for packet with expected number */
                    ask_to_resend(count_s.exp());
                    return;
                }
            }
        };

        auto ec_callb = [ptr = weak_from_this(), this](const auto &e)
        {
            if (auto p = ptr.lock())
            {
                device.notifier.error(e);
            }
        };

        return std::make_tuple(std::move(ok_callb), std::move(ec_callb));
    }

    template <typename Device_t>
    inline auto Exchanger<Device_t>::get_serial_args_helper_0()
    {
        try
        {
            /* Get iterators for new message */
            auto &msg = count_m.create();

            return get_serial_args_helper_1(msg);
        }
        catch (const Packet_controller_except &e)
        {
            device.notifier.debug(e.what());

            auto &msg = count_m.oldest();

            return get_serial_args_helper_1(msg);
        }
    }

    template <typename Device_t>
    template <typename Mqtt_msg_t>
    inline auto Exchanger<Device_t>::get_serial_args_helper_1(Mqtt_msg_t &msg)
    {
        auto msg_begin = msg.begin();
        auto msg_end = msg.end();
        msg_id = msg.id();

        // auto ok_callb = [ptr = shared_from_this(), this, id = msg_id](auto begin, auto end, auto callb) mutable
        auto ok_callb = [ptr = weak_from_this(), this](auto msg_begin, auto msg_end, auto &&callb) mutable
        {
            if (auto p = ptr.lock() && serial_to_remote())
            {
                Unique_guard lock{count_mutex};
                
                /* Retrive buffers of newly created message */
                auto &old_msg = count_m[msg_id];
                /* Set message length */
                old_msg.set_len(msg_end - msg_begin);

                lock.unlock();

                /* Send MQTT message */
                remote_d.write(
                    msg_id,
                    msg_begin,
                    msg_end,
                    [ptr = std::move(p),
                     this,
                     callb = std::forward<decltype(callb)>(callb)](char, size_t)
                    {
                        
                        
                        /* Mark last message as unused */
                        count_m[msg_id].unused();

                        try
                        {
                            /* Create new message, save id */
                            auto &msg = count_m.create();
                            msg_id = msg.id();

                            /* Set new buffer */
                            callb(msg.begin(), msg.end());
                        }
                        catch (const Packet_controller_except &e)
                        {
                            try
                            {
                                /* Use oldest one */
                                auto &msg = count_m.oldest();
                                msg_id = msg.id();

                                /* Set new buffer */
                                callb(msg.begin(), msg.end());
                            }
                            catch (const std::logic_error &e)
                            {
                                /* Shutting down connection required */
                                device.notifier.error(Exchanger_fatal_except{e.what()});

                                return;
                            }

                            device.notifier.debug(e.what());
                        }

                        /* Check if messages were ACKed */
                        if (count_m.get_not_acked() > Packet_defs::max_not_ack)
                        {
                            device.notifier.debug("Exceeded not ACKed messages...");
                        }
                    },
                    [ptr = shared_from_this(), this](const auto &e)
                    {
                        device.notifier.error(e);
                    });
            }
        };

        return std::make_tuple(msg_begin, msg_end, std::move(ok_callb));
    }

    template <typename Device_t>
    inline auto Exchanger<Device_t>::get_serial_args()
    {   
        auto [msg_begin, msg_end, ok_callb] = get_serial_args_helper_0();
        
        auto ec_callb = [ptr = weak_from_this(), this](const Serial_except &e)
        {
            if (auto p = ptr.lock())
            {
                device.notifier.error(e);
            }
        };

        return std::make_tuple(msg_begin, msg_end, std::move(ok_callb), std::move(ec_callb));
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
        typename Serial_side_ptr_t,
        typename Remote_conn_ptr_t,
        typename Serial_conn_ptr_t>
    inline Exchanger<Device_t>::Exchanger(
        Forwarder &&manager,
        Notyfier &&device.notifier,
        Device_weak_ptr_t &&device_ptr,
        Remote_conn_ptr_t &&remote_c_,
        Remote_settings_ptr_t &&remote_s,
        Remote_side_ptr_t &&remote_d,
        Serial_conn_ptr_t &&serial_c_,
        Serial_side_ptr_t &&serial_d,
        Remote_conf_port_rec &remote_rec)
        : Common_ext{std::move(manager), std::move(device.notifier), std::forward<Device_weak_ptr_t>(device_ptr)},
          remote_s{std::forward<Remote_settings_ptr_t>(remote_s)},
          remote_d{std::forward<Remote_side_ptr_t>(remote_d)},
          serial_d{std::forward<Serial_side_ptr_t>(serial_d)},
          remote_rec{remote_rec}
    {
    }

    template <typename Device_t>
    template <typename... Args_t>
    inline auto Exchanger<Device_t>::make(Args_t &&...args)
    {
        return std::make_unique<Exchanger<Device_t>>(std::forward<Args_t>(args)...);
    }

    template <typename Device_t>
    inline void Exchanger<Device_t>::start()
    {
        /* Bind exchanger to ports */
        std::apply(
            [this](auto &&...args)
            {
                remote_c.connect(std::forward<decltype(args)>(args)...);
            },
            get_remote_args());
        std::apply(
            [this](auto &&...args)
            {
                serial_c.connect(std::forward<decltype(args)>(args)...);
            },
            get_serial_args());
    }
    
    template <typename Device_t>
    inline void Exchanger<Device_t>::restart()
    {
        /* Reset packet nums*/
        count_s.reload();
        count_m.reload();

        /* Lock ports and wait for connection to be reestablished */
        comm_unlock = false;
    }
}