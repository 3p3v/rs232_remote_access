#include <Base_lock.hpp>


// void Logic::Base_lock::unlock() const
// {
// }

Logic::Base_lock::~Base_lock() = default;

std::unique_ptr<Logic::Base_lock> Logic::Base_lock_factory::create()
{
    return std::make_unique<Base_lock>();
}
