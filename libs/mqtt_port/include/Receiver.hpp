#pragma once

#include <memory>
#include <vector>
#include <Validator.hpp>

namespace Mqtt_port
{
    class Receiver //: public Validator
    {
        std::shared_ptr<Validator> validator;
    public:
        Receiver(std::shared_ptr<Validator> validator);

        void read(const std::string &channel_name, std::vector<unsigned char> data);
    };
}
