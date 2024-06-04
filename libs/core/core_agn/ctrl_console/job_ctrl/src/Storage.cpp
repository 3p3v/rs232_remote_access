#include <Storage.hpp>

namespace Job_ctrl
{
    void Storage::add_worker(Worker &worker)
    {
        ws.add_worker(worker);
    }

    void Storage::remove_worker(const Worker &worker)
    {
        ws.remove_worker(worker);
    }
}