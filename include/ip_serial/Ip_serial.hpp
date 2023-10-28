#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <Basic_controller.hpp>
#include <Ctrl_console.hpp>
#include <Setup_loader.hpp>
#include <Dyn_serial.hpp>
#include <Flow_monitor.hpp>

using namespace Cmd_ctrl::Ctrl;

class Ip_defs
{
public:
    static constexpr char* sbr {"sbr"};
    static constexpr std::string_view set_baud_rate{"set_baud_rate"};
    static constexpr std::string_view set_parity{"set_parity"};
    static constexpr std::string_view set_char_size{"set_char_size"};
    static constexpr std::string_view set_flow_ctrl{"set_flow_ctrl"};
    static constexpr std::string_view set_stop_bits{"set_stop_bits"};
};

class Ip_serial final : protected Ip_defs, public Serial_port::Ctrl_defs
{
    using Handle = Ctrl::Handle_t;

    Mqtt_port::Basic_controller controller;
    std::unique_ptr<Cmd_ctrl::Base_ctrl_console<Handle::Base_handle_intf>> console;
    std::map<std::string, std::shared_ptr<Serial_port::Serial>> serials;
    std::shared_ptr<Flow_monitor> flow_monitor;

public:
    Ip_serial::Ip_serial(Mqtt_port::Server &&server,
                         Mqtt_port::User &&user);

    template <typename Str, typename Device_t>
    void add_device(Device_t &&device);
    Serial_port::Serial &operator[](const std::string &dev_name);

    void set_baud_rate(const std::string &dev_name, const unsigned int baud_rate);
    void set_parity(const std::string &dev_name, const Parity parity);
    void set_char_size(const std::string &dev_name, unsigned int char_size);
    void set_flow_ctrl(const std::string &dev_name, const Flow_ctrl flow_ctrl);
    void set_stop_bits(const std::string &dev_name, const Stop_bits stop_bits);
    void close(const std::string &dev_name);
};

template <typename Str, typename Device_t>
void Ip_serial::add_device(Device_t &&device)
{
    auto device_ptr = std::make_shared<Device>(std::forward<Device_t>(device));

    /* Add phisical port */
    auto serial_p = serials.emplace(device.get_name(),
                                    std::make_shared<Dyn_serial>([](std::size_t write_len)
                                                                 {
                                                                    /* Not used */ },
                                                                 [this, device = device_ptr](const Data &data, std::size_t read_len)
                                                                 {
                                                                     /* Increment number of data that has to be sent */
                                                                     flow_monitor->add_out(device->get_name(), read_len);
                                                                     /* Write data to channel */
                                                                     controller.write(device->get_data_ch());
                                                                 },
                                                                 [](const unsigned int code, const std::string &err)
                                                                 {
                                                                     /* Send error to error handling object */
                                                                 }));

    /* Add channels to connect to */
    controller.add_channel(device.get_info_ch(),
                           Mqtt_port::Dyn_executor{[this, device = device_ptr](const std::string &ch_name,
                                                                               const std::vector<unsigned char>::const_iterator begin,
                                                                               const std::vector<unsigned char>::const_iterator end)
                                                   {
                                                       /* Interpret received data */
                                                       console.exec(device->get_name(), begin, end);
                                                   }});

    controller.add_channel(device.get_data_ch(),
                           Mqtt_port::Dyn_executor{[serial = &serial_p->first->second, device = device_ptr](const std::string &ch_name,
                                                                                                            const std::vector<unsigned char>::const_iterator begin,
                                                                                                            const std::vector<unsigned char>::const_iterator end)
                                                   {
                                                       /* Increment number of data that was received */
                                                       flow_monitor->add_in(device->get_name(), end - begin);
                                                       /* Write data to phisical port */
                                                       serial.write(std::vector<unsigned char>(data.begin(), data.end()), data.size());
                                                   }});
}
