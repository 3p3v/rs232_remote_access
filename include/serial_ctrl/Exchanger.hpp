#pragma once

#include <memory>
#include <Shared_helper.hpp>

/// @brief Exchange of data both ways
/// @tparam Mqtt
/// @tparam Phy
template <typename Mqtt, typename Phy>
class Exchanger
{
    std::shared_ptr<Shared_helper<Mqtt, Phy>> helper;

public:
    Exchanger(std::shared_ptr<Shared_helper<Mqtt, Phy>> helper);
};

template <typename Mqtt, typename Phy>
inline Exchanger<Mqtt, Phy>::Exchanger(std::shared_ptr<Shared_helper<Mqtt, Phy>> helper)
    : helper{std::move(helper)}
{
    /* Set MQTT object callback so it writes to physical port object */
    helper->mqtt.data_read(
        []() {

        },
        []() {

        });

    /* Set physical port object callback so it writes to MQTT object */
    helper->phy.data_read(
        []() {

        },
        []() {

        });
}
