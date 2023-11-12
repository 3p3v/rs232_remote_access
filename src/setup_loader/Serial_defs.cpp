#include <Serial_defs.hpp>
#include <Setup_except.hpp>

static constexpr std::string_view true_s{"true"};

bool Serial_defs::bool_trans(const std::string &arg)
{
    if (arg == true_s)
    {
        return true;
    }
    else if (arg == false_s)
    {
        return false;
    }
    else
    {
        throw Exception::Setup_except{"Unknown argument!"};
    }
}

bool Serial_defs::settings_known_trans(const std::string &settings_known)
{
    return bool_trans(settings_known);
}

bool Serial_defs::write_access_trans(const std::string &write_access)
{
    return bool_trans(write_access);
}

Serial_defs::~Serial_defs() = default;
