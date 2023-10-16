#include <iostream>
#include <Local_serial.hpp>
// #include <Traffic_mqtt_local.hpp>
#include <impl/Controller.hpp>

class Exec_console : public Mqtt_port::Executor
{
public:
    void exec(const Data &data)
    {
        std::cout << "Got data!" << std::endl;
    }
};

// template <typename Call_t>
// decltype(auto) make_Impl::Controller(std::string &&server_address)
// {
//     return Mqtt_port::Impl::Controller<My_callbacks>(server_address);
// }


int main(int, char **)
{
    std::cout << "Hello, from Impl::Client!\n";

    try
    {
        // auto valid = Mqtt_port::Validator();
        // valid.add_channel("console_device1", Mqtt_port::Validator::Executor_ptr(new Exec_console()));
        // valid.get_exec("console_device1")->exec(Mqtt_port::Data());

        Mqtt_port::User user{"admin", "admin", "1"};
        Mqtt_port::Server server{"127.0.0.1", "1885"};
        auto controller = Mqtt_port::Impl::Controller(server, user, 
                                                      [](const std::string &topic, const size_t len)
                                                      {

                                                      },
                                                      [](const std::string &topic, const size_t len)
                                                      {

                                                      });
        controller.add_channel<Exec_console>("console_device1");
        controller.run();

        while(1);
    }
    catch (std::exception &)
    {
    }
}
