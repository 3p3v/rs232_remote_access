#include <Monitor.hpp>
#include <Dispacher.hpp>

Monitor::Monitor(Controller &controller, const std::unordered_map<Device_ptr, Serial_pair> &devices)
    : controller{controller}, devices{devices}
{
}

static Monitor &Monitor::get()
{
    static Monitor monitor{Controller::get(), Dispacher::get_devices()};
    return monitor;
}

void Monitor::error(const Exception::Exception &except)
{

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