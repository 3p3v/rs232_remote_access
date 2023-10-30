#include <Callb.hpp>

namespace Mqtt_port
{
    Callb::Callb(Impl::Controller &controller)
        : controller{controller}
    {
    }
}