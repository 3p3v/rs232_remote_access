#include <Intf_except.hpp>

Logic::Intf_except::Intf_except(const std::string &expln)
    : expln{expln}
{
}

Logic::Intf_except::Intf_except(const char *expln)
    : expln{expln}
{
}

Logic::Intf_except::Intf_except(const std::string &expln, int code)
    : expln{expln + ' ' + std::to_string(code)}
{
}

Logic::Intf_except::Intf_except(const char *expln, int code)
    : expln{expln + std::string{' '} + std::to_string(code)}
{
}

Logic::Intf_except::Intf_except(int code)
    : expln{"Exception code: " + std::to_string(code)}
{
}

const char *Logic::Intf_except::what() const noexcept
{
    return expln.data();
}
