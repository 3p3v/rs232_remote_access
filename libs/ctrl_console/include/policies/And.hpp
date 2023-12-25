#pragma once

#include <string>

namespace Cmd_ctrl
{
    template <typename Policy_1, typename Policy_2>
    class And final
    {
    public:
        static bool validate_t(const std::string &arg);

        And() = delete;
    };

    template <typename Policy_1, typename Policy_2>
    inline bool And<Policy_1, Policy_2>::validate_t(const std::string &arg)
    {
        return Policy_1::validate_t(arg) && Policy_2::validate_t(arg);
    }
}