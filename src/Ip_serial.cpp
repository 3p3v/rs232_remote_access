#include <Ip_serial.hpp>

using namespace Cmd_ctrl::Ctrl;

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
      console{new Ctrl_console{[this](const std::string &info_ch, const std::string &data)
                               {
                                  controller.write(info_ch,
                                                   std::vector<unsigned char>(data.begin(),
                                                                              data.end()),
                                                   data.size());
                               }}}
{
   /* Add commands used to control UART settings */
   console->add_cmd("set_baud_rate",
                    Handle::Policies<Numbers_only>::Dyn_handle([](const std::string &dev_name,
                                                                  const std::string &arg)
                                                               { 
                                                            /*Set timer*/ }));
   console->add_cmd("ok_set_baud_rate",
                    Handle::Policies<Numbers_only>::Dyn_handle([this](const std::string &dev_name,
                                                                      const std::string &arg)
                                                               { serials[dev_name]->set_baud_rate(std::strtoul(arg.data(), nullptr, 0)); }));
}

void Ip_serial::set_baud_rate(const std::string &dev_name, const unsigned int baud_rate)
{
   controller.write(dev_name, set_baud_rate)
}

void Ip_serial::set_parity(const std::string &dev_name, const Parity parity)
{
}

void Ip_serial::set_char_size(const std::string &dev_name, unsigned int char_size)
{
}

void Ip_serial::set_flow_ctrl(const std::string &dev_name, const Flow_ctrl flow_ctrl)
{
}

void Ip_serial::set_stop_bits(const std::string &dev_name, const Stop_bits stop_bits)
{
}

void Ip_serial::close(const std::string &dev_name)
{
}
