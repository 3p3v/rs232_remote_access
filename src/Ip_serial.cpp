#include <Ip_serial.hpp>

Ip_serial::Ip_serial(Mqtt_port::Server &&server,
                     Mqtt_port::User &&user)
    : controller{std::move(server),
                 std::move(user),
                 /* Write handler */
                 [](const std::string &topic, const size_t len) {

                 },
                 /* Read handler */
                 [](const std::string &topic, const size_t len) {

                 }},
      console{new Cmd_ctrl::Ctrl_console{[this](const std::string &info_ch, const std::string &data)
                                         {
                                            controller.write(info_ch,
                                                             std::vector<unsigned char>(data.begin(),
                                                                                        data.end()),
                                                             data.size());
                                         }}}
{
   /* Add commands used to control UART settings */
   console->add_cmd("set_baud_rate",
                    Policies<Numbers_only>::make_ext_dyn_handle([](const std::string &dev_name,
                                                               const std::string &arg)
                                                            { 
                                                            /*Set timer*/ }));
   console->add_cmd("ok_set_baud_rate",
                    Policies<Numbers_only>::make_ext_dyn_handle([this](const std::string &dev_name,
                                                               const std::string &arg)
                                                            {
                                                               serials[dev_name]->set_baud_rate(std::strtoul(arg.data(), nullptr, 0));
                                                            }));
}

Serial_port::Serial &Ip_serial::operator[](const std::string &dev_name)
{
   // serials[dev_name]
}