#include <Ip_serial.hpp>

Ip_serial::Ip_serial(Mqtt_port::Server &&server,
                     Mqtt_port::User &&user)
    : controller{std::move(server),
                 std::move(user),
                 /* Write handler */
                 std::forward<W_handler>(w_handler)
                 /* Read handler */
                 std::forward<R_handler>(r_handler)},
      ctrl{[&controller](const std::string &info_ch, const std::string &data)
           {
               controller.write(info_ch,
                                std::vector<unsigned char>(data.begin(),
                                                           data.end()),
                                data.size());
           }}
{
    /* Add commands used to control UART settings */
    ctrl.add_cmd("set_baud_rate",
                 Policies<Numbers_only>::make_dyn_handle([](const std::string &dev_name,
                                                            const std::string &arg)
                                                         { 
                                                            /*Set timer*/ }));
    ctrl.add_cmd("ok_set_baud_rate",
                 Policies<Numbers_only>::make_dyn_handle([](const std::string &dev_name,
                                                            const std::string &arg)
                                                         { std::cout << 7 << std::endl; }));
}

Serial_port::Serial& Ip_serial::operator[](const std::string &dev_name)
{
   // serials[dev_name]
}