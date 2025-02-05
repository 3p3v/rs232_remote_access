#pragma once

#include <memory>

namespace Logic
{
    /// @brief Act as locking code from access from multiple threads
    class Base_lock
    {
    public:
        Base_lock() = default;
        Base_lock(Base_lock &&) = delete;
        Base_lock(const Base_lock &) = delete;
        Base_lock& operator=(Base_lock &&) = delete;
        Base_lock& operator=(const Base_lock &) = delete;
        virtual ~Base_lock() = 0;

    //     virtual void unlock() const;
    };

    class Base_lock_factory
    {
    public:
        virtual std::unique_ptr<Base_lock> create();
    };
}