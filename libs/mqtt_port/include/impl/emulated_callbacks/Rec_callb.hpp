#pragma once

#include <string>
#include <Emul_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        using Rec_callb_intf = Basic_emul_callb<std::string::const_iterator, std::string::const_iterator>;
        
        template <typename Ok_callb, typename Ec_callb>
        using Rec_callb = Emul_callb<Ok_callb, Ec_callb, std::string::const_iterator, std::string::const_iterator>;
    }
}
