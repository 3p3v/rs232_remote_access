#pragma once

#include <Dev_ext.hpp>

namespace Logic
{
    class Dev_remover : public Dev_ext
    {
        using Dev_ext::Dev_ext;

    public:
        void remove(Dev_num num);
    };
} // namespace Logic
