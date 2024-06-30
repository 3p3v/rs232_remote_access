#pragma once

#include <memory>

namespace Logic
{
    template <typename Impl_t>
    class Intf_base
    {
    protected:
        using Intf_base_t = Intf_base<Impl_t>;

    private:    
        Impl_t &impl; //TODO change to reference

    protected:
        /// @brief Get reference to implementation
        /// @return 
        Impl_t * const get_impl();

    public:
        /// @brief No forwarding so object won't be deleted after passing to weak_ptr
        Intf_base(Impl_t &impl);
        Intf_base(Intf_base &&) = default;
        Intf_base &operator=(Intf_base &&) = default;
        Intf_base(const Intf_base &) = default;
        Intf_base &operator=(const Intf_base &) = default;
        virtual ~Intf_base() = 0;
    };

    template <typename Impl_t>
    inline Impl_t * const Intf_base<Impl_t>::get_impl()
    {
        return &impl;
    }

    template <typename Impl_t>
    inline Intf_base<Impl_t>::Intf_base(Impl_t &impl)
        : impl{impl}
    {
    }

    template <typename Impl_t>
    Intf_base<Impl_t>::~Intf_base() = default;
}