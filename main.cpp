#include <iostream>
#include <chrono>
#include <thread>
#include <Local_serial.hpp>
// #include <Traffic_mqtt_local.hpp>
#include <Basic_controller.hpp>
#include <Ctrl_console.hpp>

class Exec_console : public Mqtt_port::Executor
{
public:
    void exec(const Data &data, size_t size)
    {
        std::cout << "Got data!" << std::endl;
    }
};

// template <typename Call_t>
// decltype(auto) make_Impl::Controller(std::string &&server_address)
// {
//     return Mqtt_port::Impl::Controller<My_callbacks>(server_address);
// }
using namespace Cmd_ctrl;

int main(int, char **)
{
    std::cout << "Hello, from Impl::Client!\n";

    try
    {
        /* Create MQTT client */
        Mqtt_port::User user{"admin", "admin", "1"};
        Mqtt_port::Server server{"127.0.0.1", "1885"};
        auto controller = Mqtt_port::Basic_controller(
            server, user,
            [](const std::string &topic, const size_t len) {

            },
            [](const std::string &topic, const size_t len) {

            });

        /* Create console handling commands from/to device */
        Cmd_ctrl::Ctrl_console ctrl{[&controller](const std::string &data)
                                    {
                                        controller.write("console_device1", std::vector<unsigned char>(data.begin(), data.end()), data.size());
                                    }};

        /* Add commands used to control UART settings */
        ctrl.add_cmd("set_baud_rate",
                     Policies<NumbersOnly>::make_dyn_handle([](const std::string &)
                                                            { std::cout << 5 << std::endl; }));
        ctrl.add_cmd("ok_set_baud_rate",
                     Policies<NumbersOnly>::make_dyn_handle([](const std::string &)
                                                            { std::cout << 7 << std::endl; }));

        /* Add channels to connect to */
        controller.add_channel("console_device1", Mqtt_port::Dyn_executor{[&ctrl](const std::vector<unsigned char>::const_iterator begin,
                                                                                  const std::vector<unsigned char>::const_iterator end)
                                                                          {
                                                                              ctrl.exec(begin, end);
                                                                          }});
        /* Connect to broker */
        controller.run();

        std::this_thread::sleep_for(std::chrono::seconds(2));
        ctrl.local_exec("set_baud_rate", "1500");
        while (1)
            ;
    }
    catch (std::exception &)
    {
    }
}
