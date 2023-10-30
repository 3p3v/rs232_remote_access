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
      console{controller}
{
   /* Add commands used to control UART settings */
   console.add_cmd(std::string{set_baud_rate_s},
                   Handle::Policies<Numbers_only>::Dyn_handle([](const std::string &dev_name,
                                                                 const std::string &arg)
                                                              { 
                                                            /*Set timer*/ }));
   console.add_cmd(Ip_defs::ok_trans(set_baud_rate_s),
                   Handle::Policies<Numbers_only>::Dyn_handle([this](const std::string &dev_name,
                                                                     const std::string &arg)
                                                              { serials[dev_name]->set_baud_rate(std::strtoul(arg.data(), nullptr, 0)); }));

   console.add_cmd(std::string{set_parity_s},
                   Handle::Policies<Numbers_only>::Dyn_handle([](const std::string &dev_name,
                                                                 const std::string &arg)
                                                              { 
                                                            /*Set timer*/ }));
   console.add_cmd(Ip_defs::ok_trans(set_parity_s),
                   Handle::Policies<Numbers_only>::Dyn_handle([this](const std::string &dev_name,
                                                                     const std::string &arg)
                                                              { serials[dev_name]->set_parity(parity_trans(arg)); }));

   console.add_cmd(std::string{set_char_size_s},
                   Handle::Policies<Numbers_only>::Dyn_handle([](const std::string &dev_name,
                                                                 const std::string &arg)
                                                              { 
                                                            /*Set timer*/ }));
   console.add_cmd(Ip_defs::ok_trans(set_char_size_s),
                   Handle::Policies<Numbers_only>::Dyn_handle([this](const std::string &dev_name,
                                                                     const std::string &arg)
                                                              { serials[dev_name]->set_char_size(std::strtoul(arg.data(), nullptr, 0)); }));

   console.add_cmd(std::string{set_flow_ctrl_s},
                   Handle::Policies<Numbers_only>::Dyn_handle([](const std::string &dev_name,
                                                                 const std::string &arg)
                                                              { 
                                                            /*Set timer*/ }));
   console.add_cmd(Ip_defs::ok_trans(set_flow_ctrl_s),
                   Handle::Policies<Numbers_only>::Dyn_handle([this](const std::string &dev_name,
                                                                     const std::string &arg)
                                                              { serials[dev_name]->set_flow_ctrl(flow_ctrl_trans(arg)); }));

   console.add_cmd(std::string{set_stop_bits_s},
                   Handle::Policies<Numbers_only>::Dyn_handle([](const std::string &dev_name,
                                                                 const std::string &arg)
                                                              { 
                                                            /*Set timer*/ }));
   console.add_cmd(Ip_defs::ok_trans(set_stop_bits_s),
                   Handle::Policies<Numbers_only>::Dyn_handle([this](const std::string &dev_name,
                                                                     const std::string &arg)
                                                              { serials[dev_name]->set_stop_bits(stop_bits_trans(arg)); }));
}

void Ip_serial::set_baud_rate(const std::string &dev_name, const unsigned int baud_rate)
{
   console.local_exec(dev_name, std::string{set_baud_rate_s}, std::to_string(baud_rate));
}

void Ip_serial::set_parity(const std::string &dev_name, const Parity parity)
{
   console.local_exec(dev_name, std::string{set_baud_rate_s}, parity_trans(parity));
}

void Ip_serial::set_char_size(const std::string &dev_name, const unsigned int char_size)
{
   console.local_exec(dev_name, std::string{set_char_size_s}, std::to_string(char_size));
}

void Ip_serial::set_flow_ctrl(const std::string &dev_name, const Flow_ctrl flow_ctrl)
{
   console.local_exec(dev_name, std::string{set_baud_rate_s}, flow_ctrl_trans(flow_ctrl));
}

void Ip_serial::set_stop_bits(const std::string &dev_name, const Stop_bits stop_bits)
{
   console.local_exec(dev_name, std::string{set_baud_rate_s}, stop_bits_trans(stop_bits));
}

void Ip_serial::close(const std::string &dev_name)
{
   // serials[dev_name]->close();
   serials.erase(dev_name);
   //TODO delete channels
}
