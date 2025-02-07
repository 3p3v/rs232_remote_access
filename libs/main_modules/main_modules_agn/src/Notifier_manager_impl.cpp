#include <Notifier_manager_impl.hpp>
#include <iostream>

void Logic::Notifier_manager_impl::error(Dev_num dev_num, const std::exception & exc) const
{
}

void Logic::Notifier_manager_impl::debug(Dev_num dev_num, const std::string &info) const
{
    if (debug)
    {
        std::clog << "[Device: " << dev_num << "]\n"; // TODO insert device name
        std::cout << info << '\n\n';
    }
}

Logic::Notifier_manager_impl::Notifier_manager_impl(bool debug)
    : debug{debug}
{
}
