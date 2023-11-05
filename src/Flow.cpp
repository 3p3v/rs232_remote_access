#include <Flow.hpp>

namespace Ip_serial
{
    size_t Flow::get()
    {
        return data;
    }

    Flow& Flow::operator+=()(unsigned int len)
    {
        data += len;
        return *this;
    }

}