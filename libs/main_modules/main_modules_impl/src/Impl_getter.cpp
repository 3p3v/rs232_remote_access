#include <Impl_getter.hpp>
#include <Impl_cont.hpp>

using namespace Logic;

Impl::Impl_getter::Impl_getter(Dev_cont &devs, Impl_cont &infos)
    : Dev_getter{devs}, infos{infos}
{
}

Impl::Impl_getter::Info_prereq_and_lock Impl::Impl_getter::get_infos_and_lock(Dev_num num) const
{
    auto info = infos.infos.find(num);

    if (info != infos.infos.end())
        return Info_prereq_and_lock{info->second, infos.info_mutex};
    else
        throw std::logic_error{"Info with this number does not exist"};
}

Impl::Impl_getter::Info_iters_and_lock Impl::Impl_getter::get_all_infos_and_lock() const
{
    return Info_iters_and_lock{
        infos.infos.begin(),
        infos.infos.end(),
        infos.info_mutex};
}
