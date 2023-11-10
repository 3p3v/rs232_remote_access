#include <Exception.hpp>

namespace Exception
{
    const char *Exception::what() const
    {
        return expln.data();
    }
}