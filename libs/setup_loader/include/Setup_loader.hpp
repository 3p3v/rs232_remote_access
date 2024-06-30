#pragma once

#include <Setup_impl.hpp>

using namespace Cmd_ctrl;
using namespace Logic;

namespace Impl
{
    /// @brief Config file reader and interpreter
    class Setup_loader final : public Setup_impl
    {
        void load_data_handler(const std::string &file_name);

    public:
        /// @brief Load data from file
        /// @param file_name 
        /// @return Reference to object containing all nessesary information to run application and construct devices
        Data_pack &load_data(const std::string &file_name) &;
        /// @brief Load data from file
        /// @param file_name 
        /// @return Object containing all nessesary information to run application and construct devices
        Data_pack load_data(const std::string &file_name) &&;

        /// @brief Get loaded data
        /// @return Reference to object containing all nessesary information to run application and construct devices
        Data_pack &get_data_pack() &;
    };
}