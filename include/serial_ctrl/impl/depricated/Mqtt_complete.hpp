#pragma once

#include <Mqtt_side.hpp>
#include <Mqtt_settings.hpp>
#include <Device.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Controller;
    }
}

class Mqtt_complete
{
    const Device &device;
    Controller &controller;

    std::unique_ptr<Mqtt_side> data_{nullptr};
    std::unique_ptr<Mqtt_settings> settings_{nullptr};

public:
    template <typename Ok_callb, typename Ec_callb>
    void data_read(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

    template <typename Ok_callb, typename Ec_callb>
    void settings_read(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

    Mqtt_side &data();

    Mqtt_settings &settings();

    Mqtt_complete(const Device &device,
                  Mqtt_port::Impl::Controller &controller);
};

template <typename Ok_callb, typename Ec_callb>
inline void Mqtt_complete::data_read(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
{
    data_ = std::make_unique<Mqtt_side>(
        device,
        controller,
        std::forward<Ok_callb>(ok_callb),
        std::forward<Ec_callb>(ec_callb));
}

template <typename Ok_callb, typename Ec_callb>
inline void Mqtt_complete::settings_read(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
{
    setings_ = std::make_unique<Mqtt_settings>(
        device,
        controller,
        std::forward<Ok_callb>(ok_callb),
        std::forward<Ec_callb>(ec_callb));
}
