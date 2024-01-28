#pragma once

#include <Ext_setter.hpp>
#include <Ctrl_interface.hpp>
#include <Set_defs.hpp>
#include <Param_defs.hpp>

using namespace Serial_port;

namespace Logic
{
    /// @brief Declares operations that can be done by a user
    class User_setter : public Ext_setter,
                        protected Set_defs,
                        protected Param_defs
    {
    public:
        /// @brief
        void set_baud_rate(const unsigned int baud_rate);
        /// @brief
        void set_parity(const Ctrl_defs::Parity parity);
        /// @brief
        void set_char_size(const unsigned int char_size);
        /// @brief
        void set_stop_bits(const Ctrl_defs::Stop_bits stop_bits);
    };
}