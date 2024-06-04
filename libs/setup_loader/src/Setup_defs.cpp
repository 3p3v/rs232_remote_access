#include <Setup_defs.hpp>
#include <stdexcept>

namespace Impl
{
    std::string Setup_defs::bool_trans(bool val) noexcept
    {
        if (val == true)
            return "true";
        else
            return "false";
    }

    bool Setup_defs::bool_trans(std::string val)
    {
        if (val == "true")
            return true;
        else if (val == "false")
            return false;
        else 
            throw std::logic_error{"Value do not match any possible argument!"};
    }

    std::string Setup_defs::write_access_trans(Remote_conf_port acc) noexcept
    {
        if (acc == Remote_conf_port::Configurable)
            return "true";
        else
            return "false";
    }

    Remote_conf_port Setup_defs::write_access_trans(std::string acc)
    {
        if (acc == "true")
            return Remote_conf_port::Configurable;
        else if (acc == "false")
            return Remote_conf_port::Non_configurable;
        else 
            throw std::logic_error{"Value do not match any possible argument!"};
    }
} // namespace Impl
