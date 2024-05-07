#include <Shared_ext.hpp>

using namespace Job_ctrl;

Logic::Remote_ext *Logic::Shared_ext::operator->() const noexcept
{
    return ext.operator->();
}

Logic::Remote_ext &Logic::Shared_ext::operator*() const noexcept
{
    return *ext;
}

bool Logic::Shared_ext::operator==(const Shared_ext& ext) const
{
    return typeid(*(this->ext)) == typeid(*ext);
}

Logic::Shared_ext::Ext_ptr_t &Logic::Shared_ext::get_ptr()
{
    return ext;
}
