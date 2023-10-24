#pragma once

#include <Local_serial.hpp>
#include <Basic_controller.hpp>
#include <Ctrl_console.hpp>
#include <Setup_loader.hpp>
#include <Dyn_serial.hpp>

class Ip_serial final
{
    Mqtt_port::Basic_controller controller;
    Cmd_ctrl::Ctrl_console ctrl;
    std::map<std::string, Serial> serials;

public:
    Ip_serial::Ip_serial(Mqtt_port::Server &&server,
                         Mqtt_port::User &&user);

    template <typename Str>
    void add_device(Device &device);
};

template <typename Str>
void Ip_serial::add_device(Device &device)
{
    /* Add phisical port */
    serials.emplace(device.get_name(),
                    Dyn_serial([]() {},
                               []() {},
                               []() {}));

    /* Add channels to connect to */
    controller.add_channel(device.get_info_ch(),
                           Mqtt_port::Dyn_executor{[&ctrl](const std::string &ch_name,
                                                           const std::vector<unsigned char>::const_iterator begin,
                                                           const std::vector<unsigned char>::const_iterator end)
                                                   {
                                                       ctrl.exec(ch_name, begin, end);
                                                   }});

    controller.add_channel(device.get_data_ch(),
                           Mqtt_port::Dyn_executor{[&controller](const std::string &ch_name,
                                                                 const std::vector<unsigned char>::const_iterator begin,
                                                                 const std::vector<unsigned char>::const_iterator end)
                                                   {
                                                       controller.write(ch_name, 
                                                                        std::vector<unsigned char>(data.begin(), data.end()), data.size());
                                                   }});
}
