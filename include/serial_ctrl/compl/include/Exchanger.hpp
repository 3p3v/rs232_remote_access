#pragma once

#include <memory>
#include <Shared_helper.hpp>
#include <sides/Mqtt_side.hpp>
#include <sides/Serial_side.hpp>
#include <Under_exchanger.hpp>

namespace Logic
{
    /// @brief Exchange of data both ways in respect of protocol
    /// @tparam Mqtt Low-level MQTT connecction object
    /// @tparam Serial Low-level serial port connecction object
    template <typename Mqtt, typename Serial>
    class Exchanger : public Under_exchanger,
                      protected Packet_defs,
                      public std::enable_shared_from_this<Exchanger>
    {
        using Shared_helper_ptr = std::shared_ptr<Shared_helper>;

        /* Shared objects */
        Shared_helper_ptr helper;

        /* MQTT connection handling */
        Mqtt_side<Mqtt> mqtt;
        /* Serial port connection handling */
        Serial_side<Serial> serial;

    public:
        /// @brief Resend all messages >= id
        /// @param id
        void resend(char id) override;

        template <typename Mqtt_t,
                  typename Serial_t,
                  typename Shared_helper_ptr_t,
                  typename =
                      std::enable_if_t<
                          !std::is_base_of_v<Mqtt,
                                             std::decay_t<Mqtt_t>>> &&
                      std::enable_if_t<
                          !std::is_base_of_v<Serial,
                                             std::decay_t<Serial_t>>> &&
                      std::enable_if_t<
                          !std::is_same_v<Shared_helper_ptr,
                                          std::decay_t<Shared_helper_ptr_t>>>>
        Exchanger(
            Mqtt_t &&mqtt,
            Serial_t &&serial,
            Shared_helper_ptr_t &&helper);
    };

    template <typename Mqtt, typename Serial>
    template <typename Mqtt_t,
              typename Serial_t,
              typename Shared_helper_ptr_t,
              typename>
    inline Exchanger<Mqtt, Serial>::Exchanger(
        Mqtt_t &&mqtt,
        Serial_t &&serial,
        Shared_helper_ptr_t &&helper)
        : mqtt{std::forward<Mqtt_t>(mqtt)},
          serial{std::forward<Serial_t>(serial)},
          helper{std::forward<Shared_helper_ptr_t>(helper)}
    {
        /* Set MQTT object callback so it writes to physical port object */
        mqtt.connect(
            [ptr = shared_from_this(), this](int num, auto begin, auto end, auto callb)
            {
                try
                {
                    /* Check number */
                    helper->counter.num_up(num);

                    if (counter.get_not_acked() > ack_after)
                        helper->setter.ack(helper->counter.exp());
                }
                catch (const std::logic_error &e)
                {
                    /* Ask for packet with expected number */
                    helper->setter.ask_to_resend(helper->counter.exp());
                    return;
                }

                try
                {
                    /* Write to serial, save callback */
                    serial.write(
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
            },
            [](const std::exception &e) {

            });

        /* Get iterators for new message */
        auto msg = helper->msgs.create([](const std::logic_error &e) {

        });
        auto msg_begin = msg.begin();
        auto msg_end = msg.end();
        auto msg_id = msg.id();

        /* Set physical port object callback so it writes to MQTT object */
        serial.connect(
            msg_begin,
            msg_end,
            [ptr = shared_from_this(), this, id = msg_id](auto begin, auto end, auto callb)
            {
                try
                {
                    /* Retrive buffers of newly created message */
                    auto old_id = id;
                    auto old_msg = helper->msgs[id];
                    auto msg_begin = old_msg.begin();
                    auto msg_end = old_msg.end();

                    /* Create new message, save id */
                    id = helper->msgs.create(
                                         [](const std::logic_error &e) {

                                         })
                             .id();

                    /* Send MQTT message */
                    mqtt.write(  
                        old_id,
                        msg_begin,
                        msg_end,
                        [ptr = shared_from_this(),
                         this,
                         id,
                         old_id,
                         callb = std::forward<decltype(callb)>(callb)](char, size_t)
                        {
                            /* Set last message as unused */
                            helper->msgs[id].unused();

                            /* Retrive next messages buffers */
                            auto msg = helper->msgs[id];

                            /* Set new buffer */
                            callb(msg.begin(), msg.end());

                            /* Check if messages were ACKed */
                            if (helper->msgs.get_not_acked() > max_not_acked)
                            {
                                // ERROR
                            }
                        },
                        [](const std::exception &e) {

                        });
                }
                catch (const std::logic_error &e)
                {
                }
            },
            [](const std::exception &e) {

            });
    }

    template <typename Mqtt, typename Serial>
    inline void Exchanger<Mqtt, Serial>::resend(char id)
    {
        auto msgs = helper->get(id);

        std::for_each(
            msgs.begin(),
            msgs.end(),
            [this](auto &m)
            {
                /* Send MQTT message */
                mqtt.write(
                    m.id,
                    m.begin(),
                    m.end(),
                    [ptr = shared_from_this(),
                     this,
                     id = m.id](char, size_t)
                    {
                        /* Set last message as unused */
                        helper->msgs[id].unused();
                    },
                    [](const std::exception &e) {

                    });
            });
    }
}