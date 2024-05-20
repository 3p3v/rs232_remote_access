#include <gtest/gtest.h>
#include <Impl_getter.hpp>
#include <Impl_adder.hpp>
#include <Impl_remover.hpp>
// #include <Exception_handler.hpp>
// #include <Debug_monitor.hpp>

using namespace ::Impl;
using namespace Logic;
using namespace Mqtt_port;

TEST(main, init_test)
{
    Dev_cont devs{};
    Impl_cont infos{};
    Worker_storage forwarder{};

    Server::Cont server_info{};
    server_info.emplace(
            Server::Option::ip,
            "192.168.0.7");
    server_info.emplace(
            Server::Option::port,
            "8888");

    User::Cont user_info{};
    user_info.emplace(
            User::Option::username,
            "Test_user");

    Impl_adder::Mqtt_controller controller{
        std::move(server_info),
        std::move(user_info)};

    Impl_getter getter{devs, infos};
    Impl_adder adder{devs, infos, forwarder, controller};
    Impl_remover remover{devs};
    // EXPECT_EQ(rec->port_settings.stop_bits, Serial_sett_impl::stop_bits);
}
