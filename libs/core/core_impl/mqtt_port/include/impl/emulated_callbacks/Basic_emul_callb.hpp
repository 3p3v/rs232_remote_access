#pragma once
#include <Basic_callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        template <typename ...Args>
        class Basic_emul_callb
        {
        public:
            virtual void ok_callb(Args&& ...args) = 0;
            virtual void ec_callb(int code) = 0;
        };
    }
}