#include <Device_initializer.hpp>
#include <Device_prereq.hpp>

Logic::Device_prereq &Logic::Device_initializer::get_dev() & noexcept
{
    return *dev;
}