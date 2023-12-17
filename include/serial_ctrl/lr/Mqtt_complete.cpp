#include <Mqtt_complete.hpp>

Mqtt_side &Mqtt_complete::data()
{
    return *data_.get();
}

Mqtt_settings &Mqtt_complete::settings()
{
    return *settings_.get();
}

Mqtt_complete::Mqtt_complete(const Device &device,
                             Mqtt_port::Impl::Controller &controller)
    : device{device},
      controller{controller}
{
}