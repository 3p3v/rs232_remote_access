#pragma once

#include <string>

namespace Cmd_ctrl
{
    class Common_defs
    {
    public:
        using Data = std::string;

        Common_defs() = default;
        Common_defs(Common_defs&&) = default;
        Common_defs& operator=(Common_defs&&) = default;
        Common_defs(Common_defs&) = default;
        Common_defs& operator=(Common_defs&) = default;
        virtual ~Common_defs() = 0;
    };
}