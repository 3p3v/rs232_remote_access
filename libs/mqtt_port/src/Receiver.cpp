#include <Receiver.hpp>

namespace Mqtt_port
{

    void Receiver::read(const std::string &channel_name, std::vector<unsigned char> data)
    {
        validator.get()->get_exec(channel_name).get()->exec(data);
    }

    Receiver::Receiver(std::shared_ptr<Validator> validator)
        : validator{std::move(validator)}
    {
    }

}