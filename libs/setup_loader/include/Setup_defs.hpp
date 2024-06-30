#pragma once

#include <string>
#include <Remote_conf_port_rec.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Definitions for reading setup file
    class Setup_defs
    {
    public:
        /// @brief Transform bool to string
        /// @param val 
        /// @return 
        static std::string bool_trans(bool val) noexcept;
        /// @brief Transform string to bool
        /// @param val 
        /// @return 
        static bool bool_trans(std::string val);

        /// @brief Transform write_access enum to string
        /// @param acc 
        /// @return 
        static std::string write_access_trans(Remote_conf_port acc) noexcept;
        /// @brief Transform string to write_access enum
        /// @param val 
        /// @return 
        static Remote_conf_port write_access_trans(std::string val);

        Setup_defs() = delete;
    };
}