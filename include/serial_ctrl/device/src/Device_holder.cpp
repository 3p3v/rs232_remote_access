#include <Device_holder.hpp>

Logic::Device_holder::Device_holder(Device_initializer_ptr && dev)
    : dev{std::move(dev)}
{
}

Logic::Device_prereq &Logic::Device_holder::get_dev() & noexcept
{
    return dev->get_dev();
}
