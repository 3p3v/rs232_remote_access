#include <Storage.hpp>



// void Job_ctrl::Storage::add_worker(Worker &&worker)
// {
//     auto ids = worker.get_ids();
//     auto w = std::make_shared<Worker>(std::move(worker));

//     std::for_each(
//         ids.cbegin(),
//         ids.cend(),
//         [&, this](const auto &id) {
//             workers.emplace(id, w);
//         });
// }
