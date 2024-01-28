#pragma once

#include <memory>
#include <Base_setter.hpp>

namespace Logic
{
    class Setter;

    class Ext_setter : public Base_setter
    {
    protected:
        virtual std::shared_ptr<Setter> shared_from_this_() = 0;
        virtual std::weak_ptr<Setter> weak_from_this_() = 0;

        /* Wait for params */
        /// @brief
        virtual void set_baud_rate_() = 0;
        /// @brief
        virtual void set_parity_() = 0;
        /// @brief
        virtual void set_char_size_() = 0;
        /// @brief
        virtual void set_stop_bits_() = 0;
    }; 
}