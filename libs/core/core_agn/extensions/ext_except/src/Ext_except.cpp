#include <Ext_except.hpp>

const char *Logic::Ext_except::what() const noexcept
{
    return expln.data();
}