#pragma once

#include <vector>
#include <stdexcept>
#include <Notyfier_defs.hpp>
#include <Forwarder.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Interface for monitor notyfier
    class Notyfication_forwarder : private Forwarder
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;

        /// @brief 
        /// @param dev 
        /// @param exc 
        void error(Dev_num dev, const std::exception &exc);
        
        /// @brief 
        /// @param dev 
        /// @param info 
        void debug(Dev_num dev, const std::string &info);

        using Forwarder::Forwarder;
    };
}