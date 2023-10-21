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
        Cmd_ctrl::Ctrl_console ctrl{};
        ctrl.add_cmd("cmd",
                     Policies<NumbersOnly>::Cmd_handle([](const std::string &)
                                                       { std::cout << 5 << std::endl; }));
        // auto valid = Mqtt_port::Validator();
        // valid.add_channel("console_device1", Mqtt_port::Validator::Executor_ptr(new Exec_console()));
        // valid.get_exec("console_device1")->exec(Mqtt_port::Data());
        int some_number = 5;

        Mqtt_port::User user{"admin", "admin", "1"};
        Mqtt_port::Server server{"127.0.0.1", "1885"};
        auto controller = Mqtt_port::Basic_controller(
            server, user,
            [&some_number](const std::string &topic, const size_t len) {

            },
            [](const std::string &topic, const size_t len) {

            });
        controller.add_channel<Exec_console>("console_device1");
        controller.add_channel<Mqtt_port::Dyn_executor>("con", [](const Mqtt_port::Executor::Data &data, size_t size){});
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
