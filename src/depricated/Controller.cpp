#include <ip_serial/Controller.hpp>

namespace Ip_serial
{
    Controller::Controller(Server::Get_cont &&server,
                           User::Get_cont &&user)
        : controller{std::move(server), std::move(user)}
    {
    }
    
    std::shared_ptr<Phy_serial::Serial_ctrl> Controller::bind_to_controller(Phy_serial::Ext_serial_ctrl &&serial_ctrl)
    {
        return std::make_shared<Phy_serial::Serial_ctrl>(serial_ctrl, controller);
    }
}