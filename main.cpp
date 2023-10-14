#include <iostream>
#include <Local_serial.hpp>
// #include <Traffic_mqtt_local.hpp>
#include <Mqtt_controller.hpp>

class Exec_console : public Mqtt_port::Executor
{
public:
    void exec(const Data &data)
    {
        std::cout << "Got data!" << std::endl;
    }
};

template <typename Call_t>
decltype(auto) make_mqtt_controller(std::string &&server_address)
{
    return Mqtt_port::Mqtt_controller<My_callbacks>(server_address);
}


int main(int, char **)
{
    std::cout << "Hello, from mqtt_client!\n";

    try
    {
        // auto valid = Mqtt_port::Validator();
        // valid.add_channel("console_device1", Mqtt_port::Validator::Executor_ptr(new Exec_console()));
        // valid.get_exec("console_device1")->exec(Mqtt_port::Data());


        auto controller = Mqtt_port::Mqtt_controller("127.0.0.1:1885");
        controller.add_channel("console_device1", Mqtt_port::Validator::Custom_exec_ptr<Exec_console>(new Exec_console()));
        controller.run();

        while(1);
    }
    catch (std::exception &)
    {
    }
}
