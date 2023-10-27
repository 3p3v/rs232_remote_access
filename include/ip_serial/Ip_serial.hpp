#pragma once

#include <memory>
#include <Basic_controller.hpp>
#include <Ctrl_console.hpp>
#include <Setup_loader.hpp>
#include <Dyn_serial.hpp>
#include <Flow_monitor.hpp>

class Ip_serial final
{
    Mqtt_port::Basic_controller controller;
    std::unique_ptr<Cmd_ctrl::Base_ctrl_console<Base_handle_intf<Extended>>> console;
    std::map<std::string, std::shared_ptr<Serial_port::Serial>> serials;
    Flow_monitor flow_monitor;

public:
    Ip_serial::Ip_serial(Mqtt_port::Server &&server,
                         Mqtt_port::User &&user);

    template <typename Str>
    void add_device(Device &device);
    Serial_port::Serial &operator[](const std::string &dev_name);
};

template <typename Str>
void Ip_serial::add_device(Device &device)
{
    /* Add phisical port */
    auto serial_p = serials.emplace(device.get_name(),
                                    std::make_shared<Dyn_serial>([](std::size_t write_len)
                                                                 {
                                                                    /* Not used */ },
                                                                 [this, device = device](const Data &data, std::size_t read_len)
                                                                 {
                                                                     /* Increment number of data that has to be sent */
                                                                     flow_monitor.add_out(device.get_name(), read_len);
                                                                     /* Write data to channel */
                                                                     controller.write(device.get_data_ch());
                                                                 },
                                                                 [](const unsigned int code, const std::string &err)
                                                                 {
                                                                     /* Send error to error handling object */
                                                                 }));

    /* Add channels to connect to */
    controller.add_channel(device.get_info_ch(),
                           Mqtt_port::Dyn_executor{[this, device = device](const std::string &ch_name,
                                                                           const std::vector<unsigned char>::const_iterator begin,
                                                                           const std::vector<unsigned char>::const_iterator end)
                                                   {
                                                       /* Increment number of data that was received */
                                                       flow_monitor.add_in(device.get_name(), end - begin);
                                                       /* Interpret received data */
                                                       console.exec(device.get_name(), begin, end);
                                                   }});

    controller.add_channel(device.get_data_ch(),
                           Mqtt_port::Dyn_executor{[serial = &serial_p->first->second](const std::string &ch_name,
                                                                                       const std::vector<unsigned char>::const_iterator begin,
                                                                                       const std::vector<unsigned char>::const_iterator end)
                                                   {
                                                       /* Write data to phisical port */
                                                       serial.write(std::vector<unsigned char>(data.begin(), data.end()), data.size());
                                                   }});
}
