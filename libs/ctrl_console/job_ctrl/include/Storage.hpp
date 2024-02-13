#pragma once

#include <Worker_storage.hpp>

namespace Job_ctrl
{
    template <typename Worker_ptr_t>
    class Storage : virtual public Worker_storage<Worker_ptr_t>
    {
    public:
        /// @brief Add new worker
        /// @param worker
        virtual void add_worker(Worker_ptr &&worker);
    };

    template <typename Worker_ptr_t>
    inline void Storage<Worker_ptr_t>::add_worker(Worker_ptr &&worker)
    {
        std::vector<Worker::Job_info> ids;
        
        if constexpr (std::is_pointer_v<T> || is_shared_ptr<T>::value || is_unique_ptr<T>::value)
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
}