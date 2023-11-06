#include <serial/Controller.hpp>

namespace Phy_serial
{
    // Serial_pair Controller::add_serial(Base_serial_ctrl &&serial_ctrl)
    // {
    //     // TODO create virtual serial

    //     /* Add phisical port */
    //     auto serial_ctrl_ = std::make_shared<Serial_ctrl>(std::move(serial_ctrl), , "CNCA0", "CNCB0");

    //     auto serial =
    //         serial->run();
    //     return std::amke_pair(std::move(serial), std::move(serial_ctrl_));
    // }

    // void Controller::close_serial(std::shared_ptr<Serial_get> &&serial_ctrl)
    // {
    //     // TODO delete created virtual port
    // }

    Controller::Com_pair Controller::create_virtual_coms()
    {
        return std::make_pair("CNCA0", "CNCB0");
    }

    
}