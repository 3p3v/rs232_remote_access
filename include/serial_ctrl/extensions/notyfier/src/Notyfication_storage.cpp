#include <Notyfication_storage.hpp>

namespace Logic
{
    void Notyfication_storage::add(Notyfication_observer &ob)
    {
        add_worker(ob);
    }

    // Notyfication_storage::Notyfication_storage(Worker_storage_ptr_t &&ws)
    //     : Storage_t{ws}
    // {
    // }

    // Notyfication_storage::Notyfication_storage(const Worker_storage_ptr_t &ws)
    //     : Storage_t{std::move(ws)}
    // {
    // }

    // Notyfication_storage::~Notyfication_storage() = default;
}
