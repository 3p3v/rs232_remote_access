#pragma once

#include <Serial.hpp>
#include <atomic>
#include <mutex>
#include <Serial_context.hpp>
#include <array>

namespace Phy_serial
{
    class Dyn_serial final : public Serial_port::Serial
    {
    public:
        template <typename Str>
        Dyn_serial(Str &&port);
    };

    template <typename Str>
    Dyn_serial::Dyn_serial(Str &&port)
        : Serial{Serial_context::shared_io_context_, std::forward<Str>(port)}
    {
        // std::lock_guard<std::mutex> guard(run_guard);
    }
}