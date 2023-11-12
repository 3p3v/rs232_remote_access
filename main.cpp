#include <iostream>
#include <chrono>
#include <thread>
// #include <Basic_controller.hpp>
#include <Ctrl_console.hpp>
#include <Setup_loader.hpp>
#include <Dispacher.hpp>
#include <Monitor.hpp>
#include <main/Controller.hpp>
#include <Setup_except.hpp>

using namespace Cmd_ctrl;

static std::string file_name{"../../resources/conf.txt"};

int main()
{
    std::cout << "Hello, from Impl::Client!\n";

    bool auto_reload{true};

    while (1)
    {
        Setup_loader::Data_pack config_data;
        
        try
        {
            /* Load config */
            config_data = Setup_loader{file_name}.load_data();
            auto_reload = config_data.auto_reload;
        }
        catch(const std::exception& e)
        {
            std::cout << "Unhandlable exception!\n";
            std::cerr << e.what() << '\n';
            return -1;
        }
        
        try
        {
            /* Init */
            auto& serial_controller = Dispacher::get_controller(std::move(config_data.server), std::move(config_data.user));
            auto& monitor = Monitor::get();
            
            std::for_each(config_data.devices.begin(), 
                          config_data.devices.end(),
                          [&serial_controller](auto &&dev_pair)
                          {
                            serial_controller.add_device(std::move(dev_pair.first), 
                                                         std::move(dev_pair.second.info),
                                                         dev_pair.second.write_access,
                                                         dev_pair.second.settings_known);
                          });

            serial_controller.run();

        }
        catch (const Exception::Exception& ec)
        {
            if (auto_reload)
            {
                std::cout << "Restarting application...\n";
            }
            else
            {
                std::cout << "Exiting...\n";
                return -1;
            }
        }
        catch (const std::exception &except)
        {
            std::cout << "Unhandlable exception!\n";
            std::cout << except.what()  << '\n';
            return -1;
        }
    }
}
