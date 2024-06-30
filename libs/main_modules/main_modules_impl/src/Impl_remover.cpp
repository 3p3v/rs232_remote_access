#include <Impl_remover.hpp>
#include <Impl_cont.hpp>

void Impl::Impl_remover::remove(Dev_num num)
{
    auto dev = devs.devs.find(num);

    if (dev != devs.devs.end())
        devs.devs.erase(dev);
    else
        throw std::logic_error{"Device with this number does not exist"};
}