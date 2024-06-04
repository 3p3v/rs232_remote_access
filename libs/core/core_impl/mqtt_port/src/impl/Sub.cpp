#include <Sub.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        bool Sub::util_callb()
        {
            if (callb)
                return true;
            else
                return false;
        }
    }
}
