#include <Serial_except.hpp>



Logic::Serial_except::Serial_except(int code)
    : Intf_except{"Exception code: " + std::to_string(code)}
{
}