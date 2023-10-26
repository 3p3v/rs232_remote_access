#pragma once

#include <memory>
#include <Local_serial.hpp>
#include <Basic_controller.hpp>
#include <Ctrl_console.hpp>
#include <Setup_loader.hpp>
#include <Dyn_serial.hpp>

class Ip_serial final
{
    Mqtt_port::Basic_controller controller;
    std::unique_ptr<Cmd_ctrl::Base_ctrl_console<Base_handle_intf<Extended>>> console;
    std::map<std::string, std::shared_ptr<Serial_port::Serial>> serials;

public:
    Ip_serial::Ip_serial(Mqtt_port::Server &&server,
                         Mqtt_port::User &&user);

    template <typename Str>
    void add_device(Device &device);
    Serial_port::Serial& operator[](const std::string &dev_name);
};

template <typename Str>
void Ip_serial::add_device(Device &device)
{
    /* Add phisical port */
    auto serial_p = serials.emplace(device.get_name(),
                    std::make_shared<Dyn_serial>([]() {},
                                                 []() {},
                                                 []() {}));

    /* Add channels to connect to */
    controller.add_channel(device.get_info_ch(),
                           Mqtt_port::Dyn_executor{[&console](const std::string &ch_name,
                                                              const std::vector<unsigned char>::const_iterator begin,
                                                              const std::vector<unsigned char>::const_iterator end)
                                                   {
                                                       console.exec(ch_name, begin, end);
                                                   }});

    controller.add_channel(device.get_data_ch(),
                           Mqtt_port::Dyn_executor{[serial = &serial_p->first->second](const std::string &ch_name,
                                                                                 const std::vector<unsigned char>::const_iterator begin,
                                                                                 const std::vector<unsigned char>::const_iterator end)
                                                   {
                                                       serial.write(std::vector<unsigned char>(data.begin(), data.end()), data.size());
                                                   }});
}
