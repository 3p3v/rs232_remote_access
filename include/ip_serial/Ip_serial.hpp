#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <Controller.hpp>
#include <Ip_console.hpp>
#include <Setup_loader.hpp>
#include <Dyn_serial.hpp>
#include <Dyn_executor.hpp>
#include <Flow_monitor.hpp>
#include <Ip_defs.hpp>

using namespace Cmd_ctrl::Ctrl;

class Ip_serial final : protected Ip_defs, public Serial_port::Ctrl_defs
{
    using Handle = Ctrl::Handle_t;

    Mqtt_port::Controller controller;
    Ip_console console;
    std::map<std::string, std::shared_ptr<Serial_port::Serial>> serials;
    std::shared_ptr<Flow_monitor> flow_monitor{new Flow_monitor{}};

public:
    Ip_serial(Mqtt_port::Server::Get_cont &&server,
              Mqtt_port::User::Get_cont &&user);

    void run();
    
    template <typename Device_t>
    void add_device(Device_t &&device);
    Serial_port::Serial &operator[](const std::string &dev_name);

    void set_baud_rate(const std::string &dev_name, const unsigned int baud_rate);
    void set_parity(const std::string &dev_name, const Parity parity);
    void set_char_size(const std::string &dev_name, const unsigned int char_size);
    void set_flow_ctrl(const std::string &dev_name, const Flow_ctrl flow_ctrl);
    void set_stop_bits(const std::string &dev_name, const Stop_bits stop_bits);
    void close(const std::string &dev_name);
};

template <typename Device_t> //, std::enable_if_t<std::is_same_v<Device_t, Device>>*>
void Ip_serial::add_device(Device_t &&device)
{
    auto device_ptr = std::make_shared<Device>(std::forward<Device_t>(device));

    /* Add phisical port */
    auto serial_p = serials.emplace(device_ptr->get_name(),
                                    std::shared_ptr<Serial_port::Serial>{new Dyn_serial{"CNCA0",
                                                                                        [](std::size_t write_len)
                                                                                        {
                                                                    /* Not used */ },
                                                                                        [this, device = device_ptr](const Serial_port::Data_interface::Data &data, std::size_t read_len)
                                                                                        {
                                                                                            /* Increment number of data that has to be sent */
                                                                                            flow_monitor->add_out(device->get_name(), read_len);
                                                                                            /* Write data to channel */
                                                                                            controller.write(device->get_data_ch(), data.cbegin(), data.cbegin() + read_len);
                                                                                        },
                                                                                        [](const unsigned int code, const std::string &err)
                                                                                        {
                                                                                            /* Send error to error handling object */
                                                                                        }}});
    serials[device_ptr->get_name()]->run();

    /* Add channels to connect to */
    controller.add_channel(device_ptr->get_info_ch(),
                           Mqtt_port::Dyn_executor{[this, device = device_ptr](const std::string &ch_name,
                                                                               const std::string::const_iterator begin,
                                                                               const std::string::const_iterator end)
                                                   {
                                                       /* Interpret received data */
                                                       console.exec(device->get_name(), begin, end);
                                                   }});

    controller.add_channel(device_ptr->get_data_ch(),
                           Mqtt_port::Dyn_executor{[this, serial = serial_p.first->second, device = device_ptr](const std::string &ch_name,
                                                                                                                const std::string::const_iterator begin,
                                                                                                                const std::string::const_iterator end)
                                                   {
                                                       /* Increment number of data that was received */
                                                       flow_monitor->add_in(device->get_name(), end - begin);
                                                       /* Write data to phisical port */
                                                       serial->write<std::string>(begin, end);
                                                   }}).connect_channels();
}
