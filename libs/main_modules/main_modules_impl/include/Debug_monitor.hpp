#pragma once

#include <Debug_observer.hpp>
#include <Impl_cont.hpp>
#include <mutex>

namespace Impl
{
    class Debug_monitor : public Debug_observer
    {
        const bool debug;

        Impl_cont &infos;

        std::mutex write_mutex;
        
        void cout_debug(Dev_num num, const std::string& info);

    public:
        Debug_monitor(Impl_cont &infos, bool debug);
    };
}