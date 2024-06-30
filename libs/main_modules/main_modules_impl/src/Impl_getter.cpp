#include <Impl_getter.hpp>
#include <Impl_cont.hpp>

using namespace Logic;

Impl::Impl_getter::Impl_getter(Impl_cont &devs)
    : Dev_getter{devs}, Info_getter{devs}
{
}

Impl::Impl_getter::Info_Dev_prereq_and_lock Impl::Impl_getter::get_and_lock(Dev_num num)
{
    auto info = Info_getter::devs.infos.find(num);
    auto dev = Info_getter::devs.devs.find(num);

    if (info != Info_getter::devs.infos.end())
        return Info_Dev_prereq_and_lock{
            *info->second, 
            dev->second.get_dev(),
            Info_getter::devs.dev_mutex
        };
    else
        throw std::logic_error{"Info with this number does not exist"};
}

Impl::Impl_getter::Const_Info_Dev_prereq_and_lock Impl::Impl_getter::get_and_lock(Dev_num num) const
{
    auto info = Info_getter::devs.infos.find(num);
    auto dev = Info_getter::devs.devs.find(num);

    if (info != Info_getter::devs.infos.end())
        return Const_Info_Dev_prereq_and_lock{
            *info->second, 
            dev->second.get_dev(),
            Info_getter::devs.dev_mutex
        };
    else
        throw std::logic_error{"Info with this number does not exist"};
}