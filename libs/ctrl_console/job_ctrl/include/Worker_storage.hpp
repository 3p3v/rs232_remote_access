#pragma once

#include <map>
#include <memory>
#include <Worker.hpp>

namespace Job_ctrl
{
    /// @brief Used for storing Workers
    template <typename Worker_ptr_t>
    class Worker_storage
    {
    public:
        using Worker_ptr = Worker_ptr_t;

    protected:
        std::multimap<Worker::Job_info, Worker_ptr> workers;

        template <typename T>
        struct is_shared_ptr : std::false_type
        {
        };
        template <typename T>
        struct is_shared_ptr<std::shared_ptr<T>> : std::true_type
        {
        };
        template <typename T>
        struct is_unique_ptr : std::false_type
        {
        };
        template <typename T>
        struct is_unique_ptr<std::unique_ptr<T>> : std::true_type
        {
        };

    public:
        Worker_storage() = default;
        Worker_storage(const Worker_storage &) = delete;
        Worker_storage &operator=(const Worker_storage &) = delete;
        Worker_storage(Worker_storage &&) = default;
        Worker_storage &operator=(Worker_storage &&) = default;
        virtual ~Worker_storage() = default;
    };
}