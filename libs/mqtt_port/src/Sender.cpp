#include <Sender.hpp>

namespace Mqtt_port
{
    Sender::Sender(std::shared_ptr<Validator> validator)
        : validator{validator}
    {
    }

    bool Sender::write(const std::string &channel_name, const Data &data, std::size_t write_len)
    {
        if (validator->validate(channel_name))
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

    // Paho_sender::Paho_sender(std::shared_ptr<mqtt::async_client> client,
    //                 std::shared_ptr<Validator> &&validator,
    //                 std::unique_ptr<mqtt::iaction_listener> &&callb)
    //     : Sender{std::move(validator)}, client{client}, callb{std::move(callb)}
    // {
    // }
    
    // void Paho_sender::write_handle(const std::string &channel_name, const Data &data, std::size_t write_len)
    // {
    //     auto msg = mqtt::make_message(channel_name, &data.begin(), write_len);
    //     client.get()->publish(msg, nullptr, *callb);
    // }

        
}