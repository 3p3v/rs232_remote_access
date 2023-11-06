#include <Flow.hpp>

size_t Flow::get() const
{
    return data;
}

Flow &Flow::operator+=(unsigned int len)
{
    data += len;
    return *this;
}
