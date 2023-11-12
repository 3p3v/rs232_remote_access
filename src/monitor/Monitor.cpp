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
        throw except;
    }

    void Monitor::wake(const Device_ptr &device)
    {

    }

    void Monitor::wake_delete(const Device_ptr &device)
    {

    }

    void Monitor::run()
    {

    }
}