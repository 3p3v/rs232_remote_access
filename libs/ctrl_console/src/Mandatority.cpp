#include <addons/Mandatority.hpp>

namespace Cmd_ctrl
{
    void Mandatority::set_mandatority(Mandatority::Type type)
    {
        this->type = type;
    }

    void Mandatority::done()
    {
        executed = true;
    }

    bool Mandatority::check()
    {
        if (type == Mandatority::Type::mandatory)
        {
            return executed;
        }
        else
        {
            return true;
        }
    }

}