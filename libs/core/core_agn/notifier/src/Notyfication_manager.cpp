#include <Notyfication_manager.hpp>



Logic::Notyfication_manager::Notyfication_manager()
    : Notyfication_storage{Notyfication_dummy::ws}, Notyfication_forwarder{Notyfication_dummy::ws}
{
}

Logic::Notyfication_dummy::~Notyfication_dummy() = default;
