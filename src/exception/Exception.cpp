#include <Exception.hpp>

namespace Exception
{
    const char *Exception::what() const
    {
        return ("An eception has occured in module: " + module + "\n Msg: " + expln).data();
    }
}