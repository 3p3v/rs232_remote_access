#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Policy
    {
    public:
        Policy() = default;
        Policy(Policy&&) = default;
        Policy& operator=(Policy&&) = default;
        Policy(Policy&) = default;
        Policy& operator=(Policy&) = default;
        virtual ~Policy() = default;

        virtual bool validate(const std::string &arg) = 0;
    };
}