#pragma once

#include <Debug_observer.hpp>
#include <Info_getter.hpp>
#include <mutex>

namespace Impl
{
    /// @brief Class for printing debug messages
    class Debug_monitor : public Debug_observer
    {
        const bool debug;

        Info_getter infos;

        std::mutex write_mutex;
        
        /// @brief Print message implementation
        /// @param num 
        /// @param info 
        void cout_debug(Dev_num num, const std::string& info);

    public:
        /// @brief 
        /// @param infos 
        /// @param debug If should print messages
        Debug_monitor(Info_getter &&infos, bool debug); // TODO Change so getter interface for infos is passed, not whole container!
    };
}