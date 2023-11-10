#include <Rec_helper.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        std::string::const_iterator Rec_helper::cbegin()
        {
            return msg->get_payload.cbegin();
        }

        std::string::const_iterator Rec_helper::cend()
        {
            return msg->get_payload.cend();
        }
    }
}