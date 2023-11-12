#pragma once

#include <Exception.hpp>

namespace Exception
{
    class Setup_except : public Exception
    {
    public:
        Setup_except(const std::string &expln);
        Setup_except(const char *expln);
    };
}