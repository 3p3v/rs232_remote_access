#include <Notyfier.hpp>

namespace Logic
{
    Logic::Notyfier::Notyfier(Dev_num dev, const Notyfication_forwarder &forwarder)
        : Notyfication_forwarder{forwarder}, dev{dev}
    {
    }

    Notyfier::Notyfier(Dev_num dev, Worker_storage &ws)
        : Notyfication_forwarder{ws}, dev{dev}
    {
    }

    void Notyfier::error(const std::exception &exc)
    {
        Notyfication_forwarder::error(dev, exc);
    }

    void Notyfier::debug(const std::string &info)
    {
        Notyfication_forwarder::debug(dev, info);
    }
}