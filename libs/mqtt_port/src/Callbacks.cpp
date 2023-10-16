#include <Callbacks.hpp>

namespace Mqtt_port
{

    Callb::Callb(Impl::Controller &controller)
        : controller{controller}
    {
    }

    O_callb::O_callb(Impl::Controller &controller)
        : Callb{controller}
    {
    }

    I_callb::I_callb(Impl::Controller &controller)
        : Callb{controller}
    {
    }

    C_callb::C_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector)
        : Callb{controller}, connector(std::move(connector))
    {
    }

}