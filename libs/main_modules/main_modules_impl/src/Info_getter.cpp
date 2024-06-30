#include <Info_getter.hpp>



Impl::Info_getter::Info_getter(Impl_cont & devs)
    : Impl_ext{devs}
{
}

Impl::Info_getter::Info_prereq_and_lock Impl::Info_getter::get_info_and_lock(Dev_num num)
{
    auto info = devs.infos.find(num);

    if (info != devs.infos.end())
        return Info_prereq_and_lock{*info->second, devs.dev_mutex};
    else
        throw std::logic_error{"Info with this number does not exist"};
}

Impl::Info_getter::Info_iters_and_lock Impl::Info_getter::get_all_infos_and_lock()
{
    return Info_iters_and_lock{
        devs.infos.begin(),
        devs.infos.end(),
        devs.dev_mutex};
}

Impl::Info_getter::Const_Info_prereq_and_lock Impl::Info_getter::get_info_and_lock(Dev_num num) const
{
    auto info = devs.infos.find(num);

    if (info != devs.infos.end())
        return Const_Info_prereq_and_lock{*info->second, devs.dev_mutex};
    else
        throw std::logic_error{"Info with this number does not exist"};
}

Impl::Info_getter::Const_Info_iters_and_lock Impl::Info_getter::get_all_infos_and_lock() const
{
    return Const_Info_iters_and_lock{
        devs.infos.cbegin(),
        devs.infos.cend(),
        devs.dev_mutex};
}