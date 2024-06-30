#include <Dev_getter.hpp>
#include <stdexcept>
#include <Dev_cont.hpp>

Logic::Dev_getter::Device_prereq_and_lock Logic::Dev_getter::get_dev_and_lock(Dev_num num)
{
    auto dev = devs.devs.find(num);

    if (dev != devs.devs.end())
        return Device_prereq_and_lock{dev->second.get_dev(), devs.dev_mutex};
    else
        throw std::logic_error{"Device with this number does not exist"};
}

Logic::Dev_getter::Device_iters_and_lock Logic::Dev_getter::get_all_devs_and_lock()
{
    return Device_iters_and_lock{
        devs.devs.begin(),
        devs.devs.end(),
        devs.dev_mutex};
}

Logic::Dev_getter::Const_Device_prereq_and_lock Logic::Dev_getter::get_dev_and_lock(Dev_num num) const
{
    auto dev = devs.devs.find(num);

    if (dev != devs.devs.end())
        return Const_Device_prereq_and_lock{dev->second.get_dev(), devs.dev_mutex};
    else
        throw std::logic_error{"Device with this number does not exist"};
}

Logic::Dev_getter::Const_Device_iters_and_lock Logic::Dev_getter::get_all_devs_and_lock() const
{
    return Const_Device_iters_and_lock{
        devs.devs.cbegin(),
        devs.devs.cend(),
        devs.dev_mutex};
}