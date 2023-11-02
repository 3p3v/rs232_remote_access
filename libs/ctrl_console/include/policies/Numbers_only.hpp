#pragma once

#include <Policy.hpp>

namespace Cmd_ctrl
{
    class Numbers_only : public Policy
    {
    public:
        bool validate(const std::string &arg) override;
        static bool validate_t(const std::string &arg);
        static bool validate_t();
    };
}