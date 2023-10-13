#pragma once

#include <memory>
#include <vector>
#include <Validator.hpp>

namespace Mqtt_port
{
    class Receiver
    {
        std::shared_ptr<Validator> validator;

    protected:
        void read(const std::string &channel_name, std::vector<unsigned char> data);

    public:
        Receiver(std::shared_ptr<Validator> validator);
    };
}
