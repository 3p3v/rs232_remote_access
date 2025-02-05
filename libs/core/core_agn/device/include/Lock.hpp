#pragma once

#include <mutex>
#include <Base_lock.hpp>

namespace Logic
{
    /// @brief Lock code from access from multiple threads
    class Lock final : public Dummy_lock
    {
        std::scoped_lock<std::mutex> lock;

    public:
        // void unlock() const override;

        Lock(std::mutex &mutex);
    };

    class Lock_factory : public Base_lock_factory
    {
    public:
        std::unique_ptr<Base_lock> create();
    };
}