#include <Sender.hpp>

namespace Mqtt_port
{
    Sender::Sender(std::shared_ptr<Validator> validator)
        : validator{validator}
    {
    }

    bool Sender::write(const std::string &channel_name, const Data &data, std::size_t write_len)
    {
        if (validator.get()->validate(channel_name))
        {
            write_handle(channel_name, data, write_len);
            return true;
        }
        else
        {
            // throw std::runtime_error("Tried to write to unknown channel " + channel_name + ".");
            return false;
        }
    }
}