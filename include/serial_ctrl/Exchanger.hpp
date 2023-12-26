#pragma once

#include <memory>
#include <Shared_helper.hpp>
#include <sides/Mqtt_side.hpp>
#include <sides/Serial_side.hpp>

namespace Logic
{
    /// @brief Exchange of data both ways in respect of protocol
    /// @tparam Mqtt Low-level MQTT connecction object
    /// @tparam Serial Low-level serial port connecction object
    template <typename Mqtt, typename Serial>
    class Exchanger : public std::enable_shared_from_this<Exchanger>
    {
        using Shared_helper_ptr = std::shared_ptr<Shared_helper>;

        /* Shared objects */
        Shared_helper_ptr helper;

        /* MQTT connection handling */
        Mqtt_side<Mqtt> mqtt;
        /* Serial port connection handling */
        Serial_side<Serial> serial;

    public:
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
        explicit Exchanger(
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
        : helper{std::forward<Mqtt_t>(helper)},
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
                    helper.counter.num_up(num);

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
        auto msg = helper.msgs.create([](const std::logic_error &e) {

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
                    auto old_id = id;

                    /* Set last message as unused */
                    helper.msgs[id].unused();
                    /* Create new message */
                    auto msg = helper.msgs.create(
                        [](const std::logic_error &e) {

                        });
                    auto msg_begin = msg.begin();
                    auto msg_end = msg.end();
                    id = msg.id();

                    mqtt.write(
                        old_id,
                        msg_begin,
                        msg_end,
                        [callb = std::forward<decltype(callb)>(callb)](char, size_t)
                        {
                            /* Set new buffer */
                            callb(msg_begin, msg_end);
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
}