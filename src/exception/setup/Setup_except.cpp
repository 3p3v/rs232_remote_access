#pragma once

#include <Setup_except.hpp>

namespace Exception
{
    Setup_except::Setup_except(const std::string &expln)
        : Exception{"SETUP", expln}
    {
    }

    Setup_except::Setup_except(const char *expln)
        : Exception{"SETUP", expln}
    {
    }
}