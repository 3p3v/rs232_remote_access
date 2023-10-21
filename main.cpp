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
        /* Create console handling commands from/to device */
        Cmd_ctrl::Ctrl_console ctrl{};
        ctrl.add_cmd("set_baud_rate",
                     Policies<NumbersOnly>::make_dyn_handle([](const std::vector<unsigned char> &)
                                                            { std::cout << 5 << std::endl; }));
        ctrl.add_cmd("baud_rate_conf",
                     Policies<NumbersOnly>::make_dyn_handle([](const std::vector<unsigned char> &)
                                                            { std::cout << 7 << std::endl; }));

        /* Run MQTT client */
        Mqtt_port::User user{"admin", "admin", "1"};
        Mqtt_port::Server server{"127.0.0.1", "1885"};
        auto controller = Mqtt_port::Basic_controller(
            server, user,
            [](const std::string &topic, const size_t len) {

            },
            [](const std::string &topic, const size_t len) {

            });
        /* Add channels to connect */
        // controller.add_channel<Exec_console>("console_device1");
        controller.add_channel("console_device1", Mqtt_port::Dyn_executor{[&ctrl](const Mqtt_port::Executor::Data &data, size_t size){
            ctrl.exec(data);
        }});
        /* Connect to broker */
        controller.run();

        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::string str{"some data to send"};
        controller.write("console_device1", Mqtt_port::Data(str.begin(), str.end()), str.size());

        while (1)
            ;
    }
    catch (std::exception &)
    {
    }
}
