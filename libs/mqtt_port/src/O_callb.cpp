#include <O_callb.hpp>

namespace Mqtt_port
{
    O_callb::O_callb(Impl::Controller &controller)
        : Callb{controller}
    {
    }
}