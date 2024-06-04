#pragma once

#include <Intf_base.hpp>

namespace Logic
{
    /// @brief Base class for all serial port related components
    /// @tparam Impl_t 
    template <typename Impl_t>
    class Serial_base : public Intf_base<Impl_t>
    {
    protected:
        using Serial_base_t = Serial_base<Impl_t>;

    private:
        using Intf_base_t::Intf_base;
    };
}