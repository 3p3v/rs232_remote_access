#pragma once

#include <Emul_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        using Conn_callb_intf = Basic_emul_callb<>;
        
        template <typename Ok_callb, typename Ec_callb>
        using Conn_callb = Emul_callb<Ok_callb, Ec_callb>;
    
        template <typename Ok_callb, typename Ec_callb>
        decltype(auto) make_conn_callb(Ok_callb &&ok_callb, Ec_callb&& ec_callb)
        {
            return std::make_unique<Conn_callb<Ok_callb, Ec_callb>>(std::forward<Ok_callb>(ok_callb), std::forward<Ec_callb>(ec_callb));
        }
    }
}
