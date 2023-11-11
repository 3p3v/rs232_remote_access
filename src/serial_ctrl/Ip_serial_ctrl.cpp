#include <Ip_serial_ctrl.hpp>
#include <Serial_ctrl_helper.hpp>
#include <impl/Controller.hpp>
#include <Mqtt_except.hpp>

namespace Ip_serial
{
  Ip_serial_ctrl::Ip_serial_ctrl(Base_serial_ctrl &&base,
                                 Mqtt_port::Impl::Controller &controller,
                                 std::shared_ptr<Serial_port::Serial> &&serial,
                                 Ip_serial::Console &console,
                                 Serial_info &&info,
                                 bool settings_known)
      : Base_serial_ctrl{std::move(base)},
        controller{controller},
        info{std::move(serial), std::move(info), monitor, settings_known},
        console{console}
  {
  }

  Ip_serial_ctrl::Ip_serial_ctrl(Base_serial_ctrl &&base, Mqtt_port::Impl::Controller &controller, Serial_ctrl_helper &&info, Ip_serial::Console &console)
      : Base_serial_ctrl{std::move(base)},
        controller{controller},
        info{std::move(info)},
        console{console}
  {
  }

  Ip_serial_ctrl::~Ip_serial_ctrl()
  {
    try
    {
      controller.unsubscribe(
          device->get_data_ch(),
          [this]()
          {
            monitor.wake_delete(device);
          },
          [](int) {

          });
    }
    catch (const mqtt::exception &e)
    {
      Exception::Mqtt_except m_e{e.what()};
      monitor.error(m_e);
    }
  };

  void Ip_serial_ctrl::connect()
  {
    /* Subscribe to info channel */
    controller.subscribe(
        device->get_info_ch(),
        qos,
        /* Successful message receive */
        [serial_ctrl = shared_from_this()](mqtt::const_message_ptr &&data)
        {
          /* Interpret received data, catch exception associated with receiving unknown/uncorrect cmds */
          serial_ctrl->exec(data->cbegin(), data->cend());
        },
        /* Unsuccessful message receive */
        [](int) {

        },
        /* Suback */
        [serial_ctrl = shared_from_this(), this]()
        {
          /* Check if device is connected */
          say_hi_();
        },
        /* Subscribe failed */
        [&monitor = this->monitor](int)
        {
          monitor.error(Exception::Mqtt_read_except{});
        });

    /* Subscribe to data channel */
    controller.subscribe(
        device->get_data_ch(),
        qos,
        [serial_ctrl = shared_from_this()](mqtt::const_message_ptr &&data)
        {
          auto beg = data->cbegin();
          auto end = data->cend();
          /* Increment number of data that was received */
          /* Write data to phisical port */
          // typename std::remove_reference_t<decltype(data->get_payload())>
          serial_ctrl->write<std::string>(beg, end, [data = std::move(data)](size_t)
                                                      {
                                                        // Copy shared_ptr of data so it's not deallocated
                                                      }); },
        [](int) {

        },
        []() {

        },
        [&monitor = this->monitor](int)
        {
          monitor.error(Exception::Mqtt_read_except{});
        });
  }

  const Serial_info &Ip_serial_ctrl::get_info() const
  {
    return info.info;
  }

  Serial_ctrl_helper &Ip_serial_ctrl::get_helper()
  {
    return info;
  }

  void Ip_serial_ctrl::say_hi_()
  {
    write_info(std::string{master_hi_s});
  }

  void Ip_serial_ctrl::say_hi()
  {
    info.timers.start_timer(std::string{master_hi_s});
  }

  void Ip_serial_ctrl::say_hi_compl()
  {
    info.timers.stop_timer(std::string{master_hi_s});
  }

  void Ip_serial_ctrl::get_settings()
  {
    write_info(std::string{get_info_s});
  }

  void Ip_serial_ctrl::set_baud_rate(const unsigned int baud_rate)
  {
    write_info(set_baud_rate_s.data(), baud_rate_trans(baud_rate));
  }

  void Ip_serial_ctrl::set_parity(const Serial_port::Ctrl_defs::Parity parity)
  {
    write_info(set_baud_rate_s.data(), parity_trans(parity));
  }

  void Ip_serial_ctrl::set_char_size(const unsigned int char_size)
  {
    write_info(set_baud_rate_s.data(), char_size_trans(char_size));
  }

  void Ip_serial_ctrl::set_flow_ctrl(const Serial_port::Ctrl_defs::Flow_ctrl flow_ctrl)
  {
    write_info(set_baud_rate_s.data(), flow_ctrl_trans(flow_ctrl));
  }

  void Ip_serial_ctrl::set_stop_bits(const Serial_port::Ctrl_defs::Stop_bits stop_bits)
  {
    write_info(set_baud_rate_s.data(), stop_bits_trans(stop_bits));
  }

  void Ip_serial_ctrl::set_baud_rate()
  {
    info.timers.stop_timer(std::string{set_baud_rate_s});
  }

  void Ip_serial_ctrl::set_parity()
  {
    info.timers.stop_timer(std::string{set_parity_s});
  }

  void Ip_serial_ctrl::set_char_size()
  {
    info.timers.stop_timer(std::string{set_char_size_s});
  }

  void Ip_serial_ctrl::set_flow_ctrl()
  {
    info.timers.stop_timer(std::string{set_flow_ctrl_s});
  }

  void Ip_serial_ctrl::set_stop_bits()
  {
    info.timers.stop_timer(std::string{set_stop_bits_s});
  }

  void Ip_serial_ctrl::set_baud_rate_compl(const std::string &arg)
  {
    info.timers.stop_timer(std::string{set_baud_rate_s});
    auto arg_ = baud_rate_trans(arg);
    info.info.baud_rate = arg_;
    info.serial->set_baud_rate(arg_);
  }

  void Ip_serial_ctrl::set_parity_compl(const std::string &arg)
  {
    info.timers.stop_timer(std::string{set_parity_s});
    auto arg_ = parity_trans(arg);
    info.info.parity = arg_;
    info.serial->set_parity(arg_);
  }

  void Ip_serial_ctrl::set_char_size_compl(const std::string &arg)
  {
    info.timers.stop_timer(std::string{set_char_size_s});
    auto arg_ = char_size_trans(arg);
    info.info.char_size = arg_;
    info.serial->set_char_size(arg_);
  }

  void Ip_serial_ctrl::set_flow_ctrl_compl(const std::string &arg)
  {
    info.timers.stop_timer(std::string{set_flow_ctrl_s});
    auto arg_ = flow_ctrl_trans(arg);
    info.info.flow_ctrl = arg_;
    info.serial->set_flow_ctrl(arg_);
  }

  void Ip_serial_ctrl::set_stop_bits_compl(const std::string &arg)
  {
    info.timers.stop_timer(std::string{set_stop_bits_s});
    auto arg_ = stop_bits_trans(arg);
    info.info.stop_bits = arg_;
    info.serial->set_stop_bits(arg_);
  }

}