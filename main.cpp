#include <iostream>
#include <chrono>
#include <thread>
// #include <Basic_controller.hpp>
#include <Ctrl_console.hpp>
#include <Setup_loader.hpp>
#include <Dispacher.hpp>
#include <Monitor.hpp>
#include <main/Controller.hpp>

using namespace Cmd_ctrl;

static std::string file_name{"../../resources/conf.txt"};

int main()
{
    std::cout << "Hello, from Impl::Client!\n";

    try
    {
        /* Init */
        auto config_data = Setup_loader{file_name}.load_data();
        auto& serial_controller = Dispacher::get_controller(std::move(config_data.server), std::move(config_data.user));
        auto& monitor = Monitor::get();
        
        serial_controller.add_device(std::move(config_data.device), false);

        serial_controller.run();

    }
    catch (std::exception &except)
    {
        std::cout << except.what();
    }
}
