#pragma once

#include <Notyfier_defs.hpp>

namespace Impl
{
    class Impl_cont;
    
    /// @brief Base for interface classes
    class Impl_ext
    {
    protected:
        Impl_cont &devs;
        
    public:
        using Dev_num = Logic::Notyfier_defs::Dev_num;

        Impl_ext(Impl_cont &devs);
        Impl_ext(Impl_ext&&) = default;
        Impl_ext& operator=(Impl_ext&&) = default;
        Impl_ext(const Impl_ext &) = default;
        Impl_ext& operator=(const Impl_ext &) = default;
        virtual ~Impl_ext() = 0;
    };
    
} // namespace Impl
