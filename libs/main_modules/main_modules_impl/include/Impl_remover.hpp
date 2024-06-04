#pragma once

#include <Dev_remover.hpp>

namespace Impl
{
    class Impl_remover : public Logic::Dev_remover
    {
        using Dev_remover::Dev_remover;
    };
} // namespace Impl
