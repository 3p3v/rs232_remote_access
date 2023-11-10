#pragma once

#include <memory>
#include <Serial_ctrl.hpp>
#include <Dyn_serial.hpp>
// #include <Ip_console.hpp>

namespace Phy_serial
{
  std::pair<const std::string &, const std::string &> Serial_ctrl::get_coms() const
  {
    return std::make_pair(com1, com2);
  }

  Serial_ctrl::Serial_ptr Serial_ctrl::connect()
  {
    auto serial = std::unique_ptr<Serial_port::Serial>(new Dyn_serial{com1,
                                                                   [serial_ctrl = shared_from_this()](const Serial_port::Data_interface::Data &data, std::size_t read_len)
                                                                   {
                                                                     /* Increment number of data that has to be sent */
                                                                     /* Write data to channel */
                                                                     serial_ctrl->write(data.cbegin(), data.cbegin() + read_len);
                                                                   },
                                                                   [](const unsigned int code, const std::string &err)
                                                                   {
                                                                     /* Send error to error handling object */
                                                                   }});

    serial->run();

    this->serial.reset(serial);

    return std::move(serial);
  }

}