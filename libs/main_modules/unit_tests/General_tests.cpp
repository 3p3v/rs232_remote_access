#include <gtest/gtest.h>
#include <Impl_getter.hpp>
#include <Impl_adder.hpp>
#include <Impl_remover.hpp>
// #include <Exception_handler.hpp>
// #include <Debug_monitor.hpp>

using namespace ::Impl;
using namespace Logic;
using namespace Mqtt_port;

TEST(general, init_test)
{
    Dev_cont devs{};            // Devices of unknown type
    Impl_cont infos{};          // Additional, implementation-related informatin about devices
    Worker_storage notifyer{};  // Information about debug and errors

    Server::Cont server_info{
        std::make_pair(Server::Option::ip, "127.0.0.1"),
        std::make_pair(Server::Option::port, "8883"),
        std::make_pair(Server::Option::cert, "./resources/server.crt")};

    User::Cont user_info{
        std::make_pair(User::Option::id, "0"),
        std::make_pair(User::Option::username, "admin"),
        std::make_pair(User::Option::password, "admin")};

    Impl_adder::Mqtt_controller controller{
        std::move(server_info),
        std::move(user_info)};

    /* Control over devices */
    Impl_getter getter{devs, infos};
    Impl_adder adder{devs, infos, notifyer, controller};
    Impl_remover remover{devs};
    
    /* Error handling */


    /* Information for user */

}
