#include <Basic_controller.hpp>

namespace Mqtt_port
{
    void Basic_controller::run()
    {
        started = true;
        if (!connector->load_channels())
            throw std::logic_error("No channels to connect were specyfied!");

        run_handle();
    }

    Basic_controller::Basic_controller()
        : validator{new Validator{}},
          receiver{new Receiver{validator}},
          connector{new Connector{validator}}
    {
    }

    void Basic_controller::write(const std::string &channel_name, const Data &data, std::size_t write_len)
    {
        if (validator->validate(channel_name))
        {
            write_handle(channel_name, data, write_len);
        }
        else
        {
            throw std::runtime_error("Tried to write to unknown channel " + channel_name + ".");
        }
    }

}