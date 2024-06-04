#include <gtest/gtest.h>
#include <Setup_loader.hpp>

using namespace ::Impl;
using namespace Logic;
using namespace Mqtt_port;

TEST(setup_loader, init_test)
{
    auto pack = Setup_loader{}.load_data("./test_setup.conf");

    /* Mqtt */
    EXPECT_EQ(pack.server.get(Server::Option::ip), "127.0.0.1");
    EXPECT_EQ(pack.server.get(Server::Option::port), "8888");
    EXPECT_EQ(pack.server.get(Server::Option::pem), "my_pem.pem");
    EXPECT_EQ(pack.server.get(Server::Option::cert), "server.crt");
    EXPECT_EQ(pack.user.get(User::Option::username), "admin");
    EXPECT_EQ(pack.user.get(User::Option::password), "admin2");
    
    /* General */
    EXPECT_EQ(pack.app_opts.close_on_data_loss, false);
    EXPECT_EQ(pack.app_opts.close_on_timeout, false);
    EXPECT_EQ(pack.app_opts.debug, true);
 
    /* Device */
    EXPECT_EQ(pack.devices.size(), 1);
    auto &dev = pack.devices.at(0); 
    EXPECT_EQ(dev.write_access, Remote_conf_port::Non_configurable);
    EXPECT_EQ(dev.info.data_ch, Topic_defs::data_ch_trans("XX:XX:XX:XX:XX:XX"));
    EXPECT_EQ(dev.info.port, "COM0");
    EXPECT_EQ(pack.devices.at(0).sett.baud_rate, 15200);
    EXPECT_EQ(pack.devices.at(0).sett.parity, Port_settings::Parity::even);
}

TEST(setup_loader, mandatory_not_supplied)
{
    EXPECT_THROW(
        {
            auto pack = Setup_loader{}.load_data("./invalid_setup.conf");
        },
        std::logic_error
    );
}