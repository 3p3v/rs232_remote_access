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
    auto serial = std::shared_ptr<Serial_port::Serial>(new Dyn_serial{com1,
                                                                   [serial_ctrl = shared_from_this(), this](Serial_port::Serial::Data::iterator data, std::size_t read_len)
                                                                   {                                                                    
                                                                     /* Increment number of data that has to be sent */
                                                                     /* Write data to channel */
                                                                     msgs->operator[](master_counter->exp()).data_len = read_len;
                                                                     write(data, data + read_len);
                                                                   },
                                                                   [](const unsigned int code, const std::string &err)
                                                                   {
                                                                     /* Send error to error handling object */
                                                                   }});
    /* Set message number */
    auto &msg = msgs->first_free();
    msg.id = master_counter->exp();

    /* Run serial */
    serial->run<Serial_port::Serial::Data>(std::begin(msg.data), msg.max_size);

    this->serial = serial;

    return std::move(serial);
  }

}