#pragma once

#include <mqtt/async_client.hpp>
#include <Rec_helper_intf.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Rec_helper final : public Rec_helper_intf
        {
        private:
            mqtt::const_message_ptr msg;

        public:
            Rec_helper(mqtt::const_message_ptr &&msg);

            std::string::const_iterator cbegin() override;
            std::string::const_iterator cend() override;
        };
    }
}