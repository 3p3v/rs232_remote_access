#include <Ip_serial_ctrl.hpp>
#include <Serial_ctrl_helper.hpp>
#include <impl/Controller.hpp>
// #include <Rec_callb_intf.hpp>

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
      controller.unsubscribe(device->get_data_ch(), 
                             [this]()
                             {
                              monitor.wake_delete(device);
                             },
                             [](int)
                             {

                             });
    }
    catch(const std::exception& e)
    {
      monitor.error(Errors::mqtt, e.what());
    }
  };

  void Ip_serial_ctrl::connect()
  {
    /* Subscribe to info channel */
    controller.subscribe(
        device->get_info_ch(),
        qos,
        /* Successful message receive */
        [serial_ctrl = shared_from_this()](auto &&data, size_t size)
        {
          /* Interpret received data, catch exception associated with receiving unknown/uncorrect cmds */
          try
          {
            /* Interpret received data */
            serial_ctrl->exec(data->cbegin(), data->cend()); 
          }
          catch(const std::exception& e)
          {
            serial_ctrl->monitor.error(Errors::cmds, e.what());
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
        [serial_ctrl = shared_from_this()](auto &&data, size_t size)
        {
          auto beg = data->cbegin();
          auto end = data->cend();
          /* Increment number of data that was received */
          /* Write data to phisical port */
          serial_ctrl->write<std::string>(beg, end, [data](size_t)
                                                      {
                                                        // Copy shared_ptr of data so it's not deallocated
                                                      }); },
        [](int) 
        {

        });

  }

  const Serial_info &Ip_serial_ctrl::get_info() const
  {
    return info.info;
  }
}