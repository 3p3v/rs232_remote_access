#pragma once

#include <Exception.hpp>

namespace Exception
{
    class Num_except : public Exception
    {
    public:
        Num_except(const std::string &expln);
        Num_except(const char *expln);
    };
}