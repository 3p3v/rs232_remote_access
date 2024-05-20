#include <Mqtt_impl.hpp>

namespace Impl
{
    Mqtt_impl::Mqtt_impl(Remote_info &info, Mqtt_controller &controller)
        : info{info}, controller{controller}
    {
    }
}