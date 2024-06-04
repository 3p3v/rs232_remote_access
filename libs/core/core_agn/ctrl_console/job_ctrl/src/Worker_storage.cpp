#include <Worker_storage.hpp>

namespace Job_ctrl
{
    bool Worker_storage::is_same_worker(const Worker &w0, const Worker &w1)
    {
        return true;
    }

    void Worker_storage::add_worker(Worker &worker)
    {
        std::vector<Worker::Job_info> ids = worker.get_ids();

        std::for_each(
            ids.cbegin(),
            ids.cend(),
            [&, this](const auto &id)
            {
                workers.emplace(id, worker);
            });
    }

    void Worker_storage::remove_worker(const Worker &worker)
    {
        auto w_beg = std::find_if(
            workers.begin(),
            workers.end(),
            [&, this](auto &r)
            {
                if (is_same_worker(r.second, worker))
                    return true;
                else
                    return false;
            }
        );
        
        auto w_end = std::find_if_not(
            w_beg,
            workers.end(),
            [&, this](auto &r)
            {
                if (is_same_worker(r.second, worker))
                    return true;
                else
                    return false;
            });

        workers.erase(w_beg, w_end);
    }
}