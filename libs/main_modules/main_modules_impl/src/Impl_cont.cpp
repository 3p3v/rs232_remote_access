#include <Impl_cont.hpp>

Impl::Impl_cont::Impl_cont(Impl_cont &&ic)
    : infos{std::move(ic.infos)}, num{ic.num}
{
}

Impl::Impl_cont &Impl::Impl_cont::operator=(Impl_cont &&ic)
{
    *this = Impl_cont{std::move(ic)};
    return *this;
}
