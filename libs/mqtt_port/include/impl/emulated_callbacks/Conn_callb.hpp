#pragma once

#include <Emul_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        using Conn_callb_intf = Basic_emul_callb<>;
        
        template <typename Ok_callb, typename Ec_callb>
        using Conn_callb = Emul_callb<Ok_callb, Ec_callb>;
    }
}
