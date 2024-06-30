#include <Impl_cont.hpp>

Impl::Impl_cont::Impl_cont(Impl_cont &&ic)
    : Dev_cont{std::move(ic)}, infos{std::move(ic.infos)}
{
}

Impl::Impl_cont &Impl::Impl_cont::operator=(Impl_cont &&ic)
{
    devs = std::move(ic.devs);
    num = ic.num;
    infos = std::move(ic.infos);
    
    return *this;
}
