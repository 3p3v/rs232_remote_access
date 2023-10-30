#include <iostream>
#include <chrono>
#include <thread>
// #include <Local_serial.hpp>
#include <Ip_serial.hpp>
#include <Basic_controller.hpp>
#include <Ctrl_console.hpp>
#include <Setup_loader.hpp>

using namespace Cmd_ctrl;

static constexpr std::string_view file_name{"conf.txt"};

int main()
{
    std::cout << "Hello, from Impl::Client!\n";

    try
    {
        auto config_data = Setup_loader{file_name}.load_data();

        Ip_serial serial_controller{std::move(config_data.server), std::move(config_data.user)};


        // Mqtt_port::Controller controller{config_data.server, config_data.user,
        //                                        /* Write handler */
        //                                        [](const std::string &topic, const size_t len) {

        //                                        },
        //                                        /* Read handler */
        //                                        [](const std::string &topic, const size_t len) {

        //                                        }};

        // /* Create console handling commands from/to device */
        // Cmd_ctrl::Ctrl_console ctrl{[&controller, &config_data](const std::string &data)
        //                             {
        //                                 controller.write(config_data.device.get_info_ch(), 
        //                                                  std::vector<unsigned char>(data.begin(), data.end()), data.size());
        //                             }};

        // /* Add commands used to control UART settings */
        // ctrl.add_cmd("set_baud_rate",
        //              Policies<Numbers_only>::make_dyn_handle([](const std::string &dev_name, 
        //                                                         const std::string &arg)
        //                                                      { /*Set timer*/ }));
        // ctrl.add_cmd("ok_set_baud_rate",
        //              Policies<Numbers_only>::make_dyn_handle([](const std::string &dev_name, 
        //                                                         const std::string &arg)
        //                                                      { std::cout << 7 << std::endl; }));

        // /* Add channels to connect to */
        // controller.add_channel(config_data.device.get_info_ch(),
        //                        Mqtt_port::Dyn_executor{[&ctrl](const std::string& dev_name,
        //                                                        const std::vector<unsigned char>::const_iterator begin,
        //                                                        const std::vector<unsigned char>::const_iterator end)
        //                                                {
        //                                                    ctrl.exec(dev_name, begin, end);
        //                                                }});

        // controller.add_channel(config_data.device.get_data_ch(),
        //                        Mqtt_port::Dyn_executor{[&ctrl](const std::string& dev_name,
        //                                                        const std::vector<unsigned char>::const_iterator begin,
        //                                                        const std::vector<unsigned char>::const_iterator end)
        //                                                {
        //                                                 //    ctrl.exec(begin, end);
        //                                                }});
        // /* Connect to broker */
        // controller.run();

        // std::this_thread::sleep_for(std::chrono::seconds(2));
        // ctrl.local_exec("set_baud_rate", "1500");
        while (1)
            ;
    }
    catch (std::exception &)
    {
    }
}
