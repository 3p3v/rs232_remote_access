#include <Ip_serial_ctrl.hpp>
#include <Serial_ctrl_helper.hpp>

namespace Ip_serial
{
  Ip_serial_ctrl::Ip_serial_ctrl(Base_serial_ctrl &&base,
                                 Mqtt_port::Impl::Controller &controller,
                                 std::unique_ptr<Serial_port::Serial> &&serial,
                                 Ip_serial::Console &console,
                                 Serial_info &&info,
                                 bool settings_known)
      : Base_serial_ctrl{std::move(base)},
        controller{controller},
        info{std::move(serial), std::move(info), settings_known},
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
    monitor.wake_delete(device);
  };

  Ip_serial_ctrl::Ip_serial_ctrl_ptr Ip_serial_ctrl::connect() &&
  {
    controller.subscribe(
        device->get_info_ch(),
        qos,
        [serial_ctrl = shared_from_this()](const std::string::const_iterator begin, const std::string::const_iterator end)
        {
                /* Interpret received data */
                serial_ctrl->exec(begin, end); },
        [](int, std::string &) {});

    controller.subscribe(
        device->get_data_ch(),
        qos,
        [serial_ctrl = shared_from_this()](const std::string::const_iterator begin, const std::string::const_iterator end)
        {
                /* Increment number of data that was received */
                /* Write data to phisical port */
                serial_ctrl->write<std::string>(begin, end); },
        [](int, std::string &) {});

    return shared_from_this();
  }

  const Serial_info &Ip_serial_ctrl::get_info() const
  {
    return info.info;
  }
}