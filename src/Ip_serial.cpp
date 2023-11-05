#include <Ip_serial.hpp>
#include <Policy.hpp>
#include <policies/Numbers_only.hpp>

using namespace Cmd_ctrl::Ctrl;

Ip_serial::Ip_serial(Mqtt_port::Server::Get_cont &&server,
                     Mqtt_port::User::Get_cont &&user)
    : controller{std::move(server),
                 std::move(user)},
      console{controller}
{
   
}

void Ip_serial::run()
{
   controller.run();
   Serial_context::run();
}

void Ip_serial::set_baud_rate(const std::string &dev_name, const unsigned int baud_rate)
{
   /*Set timer*///TODO set callback
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
   // TODO delete channels
}
