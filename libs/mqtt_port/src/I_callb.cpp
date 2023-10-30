#include <I_callb.hpp>

namespace Mqtt_port
{
    C_callb::C_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector)
        : Callb{controller}, connector(std::move(connector))
    {
    }
}