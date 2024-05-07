#pragma once

#include <Worker_storage.hpp>

namespace Job_ctrl
{
    template <typename Worker_ptr_type>
    class Storage : virtual public Worker_storage<Worker_ptr_type>
    {
    public:
        /// @brief Add new worker
        /// @param worker
        template <typename Worker_ptr_t>
        void add_worker(Worker_ptr_t &&worker);

        /// @brief Remove worker 
        /// @param worker 
        void remove_worker(const Worker_ptr_type& worker);

        Storage() = default;
        Storage(Storage&&) = default;
        Storage& operator=(Storage&&) = default;
        Storage(const Storage&) = default;
        Storage& operator=(const Storage&) = default;
        ~Storage() = 0;
    };

    template <typename Worker_ptr_type>
    template <typename Worker_ptr_t>
    inline void Storage<Worker_ptr_type>::add_worker(Worker_ptr_t &&worker)
    {
        std::vector<Worker::Job_info> ids;
        
        if constexpr (std::is_pointer_v<Worker_ptr_type> || is_shared_ptr<Worker_ptr_type>::value || is_unique_ptr<Worker_ptr_type>::value)
        {
            ids = worker->get_ids();
        }
        else
        {
            ids = worker.get_ids();
        }

        std::for_each(
            ids.cbegin(),
            ids.cend(),
            [&, this](const auto &id)
            {
                workers.emplace(id, worker);
            });
    }

    template <typename Worker_ptr_type>
    inline void Storage<Worker_ptr_type>::remove_worker(const Worker_ptr_type &worker)
    {
        auto w_beg = workers.find(worker);
        
        auto w_end = std::find_if_not(
            w_beg,
            workers.end(),
            [worker = &worker](const auto& r)
            {
                if (r.second == worker)
                    return true;
                else 
                    return false;
            }
        );

        workers.remove(w_beg, w_end);
    }

    template <typename Worker_ptr_type>
    Storage<Worker_ptr_type>::~Storage() = default;
}