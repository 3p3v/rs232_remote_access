#pragma once

#include <Impl_ext.hpp>
#include <Dev_remover.hpp>

namespace Impl
{
    /// @brief Interface for removing devices
    class Impl_remover final : public Impl_ext, public Logic::Dev_remover
    {
        using Impl_ext::Impl_ext;
    public:
        using Dev_num = Dev_remover::Dev_num;

        /// @brief Remove device with given number
        /// @param num Devices number
        void remove(Dev_num num) override;
    };
} // namespace Impl
