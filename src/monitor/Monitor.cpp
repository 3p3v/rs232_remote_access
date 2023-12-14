#include <Monitor.hpp>
#include <main/Controller.hpp>

namespace Main_serial
{
    Monitor::Monitor(Main_serial::Controller &controller, const std::unordered_map<Device_ptr, Serial_pair> &devices)
        : controller{controller}, devices{devices}
    {
    }

    void Monitor::error(const Exception::Exception &except)
    {
        try
        {
            throw except;
        }
        catch(const Exception::Cmds_bad_cmd& e)
        {
            std::cerr << e.what() << '\n';
            throw e;
        }
        catch(const Exception::Cmds_no_packet& e)
        {
            std::cerr << e.what() << '\n';
            if (app_opts.close_on_data_loss)
                throw e;
        }
        catch(const Exception::Cmds_timeout& e)
        {
            std::cerr << e.what() << '\n';
            if (app_opts.close_on_timeout)
                throw e;
        }
        catch(const Exception::Cmds_except& e)
        {
            std::cerr << e.what() << '\n';
            throw e;
        }
        catch(const Exception::Exception& e)
        {
            std::cerr << e.what() << '\n';
            throw e;
        }
        
    }

    void Monitor::wake(const Device_ptr &device)
    {
        
    }

    void Monitor::wake_delete(const Device_ptr &device)
    {
        std::cout << "Device: " << device->get_data_ch() << " has been deleted." << '\n';
    }

    void Monitor::run()
    {

    }
    void Monitor::set_opts(Setup_loader::App_opts &&app_opts)
    {
        this->app_opts = std::move(app_opts);
    }
}