#include <Setup_console.hpp>
#include <algorithm>
#include <stdexcept>

void Impl::Setup_console::mark(const std::string &name) const noexcept
{
    auto itr = run_stat.find(name);

    itr->second.executed = true;
}

void Impl::Setup_console::check()
{
    std::for_each(
        run_stat.cbegin(),
        run_stat.cend(),
        [](const auto &com_stat)
        {
            auto &stat = com_stat.second;

            if (stat.mandatority == Mandatority::Mandatory && stat.executed == false)
                throw std::logic_error{"Mandatory argument for " + com_stat.first + " was not given!"};
        }
    );
}

Impl::Setup_console::Setup_console()
    : c_storage{cc}, c_exec{cc}
{
}

Impl::Setup_console::Stat::Stat(Mandatority mandatority)
    : mandatority{mandatority}
{
}
