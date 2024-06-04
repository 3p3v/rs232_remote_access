#include <Receiver.hpp>

namespace Mqtt_port
{

    void Receiver::read(const std::string &channel_name, std::vector<unsigned char> data)
    {
        if (validator->validate(channel_name))
            validator->get_exec(channel_name).get()->exec(data);
        else
            throw std::logic_error("Somehow tried to write to channel that client was not connected to!");
    }

    Receiver::Receiver(std::shared_ptr<Validator> validator)
        : validator{std::move(validator)}
    {
    }

}