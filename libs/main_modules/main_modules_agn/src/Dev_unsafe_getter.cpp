#include <Dev_unsafe_getter.hpp>
#include <stdexcept>

Logic::Device_prereq &Logic::Dev_unsafe_getter::get_dev(Dev_num num)
{
    auto dev = devs.devs.find(num);

    if (dev != devs.devs.end())
        return dev->second.get_dev();
    else
        throw std::logic_error{"Device with this number does not exist"};
}

Logic::Dev_unsafe_getter::Device_iters Logic::Dev_unsafe_getter::get_all_devs()
{
    return Device_iters{
        devs.devs.begin(),
        devs.devs.end()};
}

const Logic::Device_prereq & Logic::Dev_unsafe_getter::get_dev(Dev_num num) const
{
    auto dev = devs.devs.find(num);

    if (dev != devs.devs.end())
        return dev->second.get_dev();
    else
        throw std::logic_error{"Device with this number does not exist"};
}

Logic::Dev_unsafe_getter::Const_device_iters Logic::Dev_unsafe_getter::get_all_devs() const
{
    return Const_device_iters{
        devs.devs.cbegin(),
        devs.devs.cend()};
}