#include <main/Controller.hpp>
#include <Serial_ctrl.hpp>
#include <Ip_serial_ctrl.hpp>
#include <Serial_context.hpp>
#include <Dispacher.hpp>

namespace Main_serial
{
   Controller::Controller(Mqtt_port::Server::Get_cont &&server,
                          Mqtt_port::User::Get_cont &&user,
                          std::unordered_map<Device_ptr, Serial_pair> &devices)
      : devices{devices}, ip_controller{std::move(server), std::move(user)}
   {
   }

   void Controller::add_device(Device &&device, bool write_access)
   {
      add_device(std::move(device), Serial_info{}, write_access, false);
   }

   void Controller::add_device(Device &&device, Serial_info &&info, bool write_access)
   {
      add_device(std::move(device), std::move(info), write_access, true);
   }

   void Controller::add_device(Device &&device, Serial_info &&info, bool write_access, bool settings_known)
   {
      // Device shared between monitor and serials
      auto device_ptr = std::make_shared<Device>(std::forward<Device>(device));

      // Base objects containing information shared resources
      Base_serial_ctrl serial_base{device_ptr};

      // Create virtual com ports (com0com)
      auto coms = com_controller.create_virtual_coms();

      // Serial connected to virtual serial
      auto serial_ctrl_pair = std::make_shared<Phy_serial::Serial_ctrl>(serial_base,
                                                                   ip_controller,
                                                                   std::move(coms.first),
                                                                   std::move(coms.second),
                                                                   write_access);
      auto serial_ptr = serial_ctrl_pair->connect();

      // Serial connected to mqtt broker
      auto ip_serial = std::make_shared<Ip_serial::Ip_serial_ctrl>(std::move(serial_base),
                                                                   ip_controller,
                                                                   std::move(serial_ptr),
                                                                   cmds_console,
                                                                   std::move(info),
                                                                   settings_known);
      ip_serial->connect();

      // Add serial to monitor
      devices.emplace(std::move(device_ptr), std::make_pair(std::move(ip_serial), std::move(serial_ctrl_pair)));
   }

   void Controller::run()
   {      
      try
      {
         ip_controller.connect([]()
                            {

                            },
                            [](int)
                            {

                            },
                            [](int)
                            {

                            });
      }
      catch(const mqtt::exception& e)
      {
         Dispacher::get().error(Exception::Mqtt_except{e});
      }
      
      
      try
      {
         Phy_serial::Serial_context::run();
      }
      catch(const boost::exception& e)
      {
        Dispacher::get().error(Exception::Serial_except{e});
      }
      
      
   }

   void Controller::set_baud_rate(const std::string &dev_name, const unsigned int baud_rate)
   {
      // devices[dev_name]->
   }

   void Controller::set_parity(const std::string &dev_name, const Parity parity)
   {
      // console.local_exec(dev_name, std::string{set_baud_rate_s}, parity_trans(parity));
   }

   void Controller::set_char_size(const std::string &dev_name, const unsigned int char_size)
   {
      // console.local_exec(dev_name, std::string{set_char_size_s}, std::to_string(char_size));
   }

   void Controller::set_flow_ctrl(const std::string &dev_name, const Flow_ctrl flow_ctrl)
   {
      // console.local_exec(dev_name, std::string{set_baud_rate_s}, flow_ctrl_trans(flow_ctrl));
   }

   void Controller::set_stop_bits(const std::string &dev_name, const Stop_bits stop_bits)
   {
      // console.local_exec(dev_name, std::string{set_baud_rate_s}, stop_bits_trans(stop_bits));
   }

   void Controller::disconnect_device(const std::string &dev_name)
   {
      // serials[dev_name]->close();
      // serials.erase(dev_name);
      // TODO delete channels
   }

   void Controller::close()
   {

   }
}
