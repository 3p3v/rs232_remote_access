#pragma once

#include <string>
#include <Remote_conf_port_rec.hpp>

using namespace Logic;

namespace Impl
{
    class Setup_defs
    {
    public:
        static std::string bool_trans(bool val) noexcept;
        static bool bool_trans(std::string val);

        static std::string write_access_trans(Remote_conf_port acc) noexcept;
        static Remote_conf_port write_access_trans(std::string val);

        Setup_defs() = delete;
    };
}