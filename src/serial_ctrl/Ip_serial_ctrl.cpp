#include <Ip_serial_ctrl.hpp>
#include <Serial_ctrl_helper.hpp>
#include <impl/Controller.hpp>

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
    try
    {
      controller.unsubscribe(channel_name, 
                             [&monitor]()
                             {
                              monitor.wake_delete(device);
                             },
                             [](int)
                             {

                             });
    }
    catch(const std::exception& e)
    {
      monitor.error(e.what());
    }
  };

  void Ip_serial_ctrl::connect()
  {
    /* Subscribe to info channel */
    controller.subscribe(
        device->get_info_ch(),
        qos,
        /* Successful message receive */
        [serial_ctrl = shared_from_this()](const std::string::const_iterator begin, const std::string::const_iterator end)
        {
          /* Interpret received data, catch exception associated with receiving unknown/uncorrect cmds */
          try
          {
            /* Interpret received data */
            serial_ctrl->exec(begin, end); 
          }
          catch(const std::exception& e)
          {
            // monitor.error(e.what());
            throw e;
          }
        },
        /* Unsuccessful message receive */
        [](int) 
        {

        },
        /* Suback */
        [serial_ctrl = shared_from_this()]()
        {
          /* Check if device is connected */
          serial_ctrl->write_info("M_HI", 
                                  [](size_t)
                                  {
                                    /* Set "M_HI" timer */
                                  }, 
                                  [](int)
                                  {
                                    
                                  });
        },
        /* Subscribe failed */
        [](int)
        {

        });

    /* Subscribe to data channel */
    controller.subscribe(
        device->get_data_ch(),
        qos,
        [serial_ctrl = shared_from_this()](const std::string::const_iterator begin, const std::string::const_iterator end)
        {
          /* Increment number of data that was received */
          /* Write data to phisical port */
          serial_ctrl->write<std::string>(begin, end); },
        [](int) 
        {

        });

  }

  const Serial_info &Ip_serial_ctrl::get_info() const
  {
    return info.info;
  }
}