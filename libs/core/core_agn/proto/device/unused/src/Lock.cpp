#include <Lock.hpp>


// void Logic::Lock::unlock() const
// {
// }

Logic::Lock::Lock(std::mutex &mutex)
    : lock{mutex}
{
}

std::unique_ptr<Logic::Base_lock> Logic::Lock_factory::create()
{
    return std::make_unique<Lock>(mutex);
}
