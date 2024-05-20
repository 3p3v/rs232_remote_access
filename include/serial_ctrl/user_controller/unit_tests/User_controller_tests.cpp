#include <gtest/gtest.h>
#include <User_controller.hpp>
#include <Process_full.hpp>
#include <Remote_dev.hpp>
#include <Notyfication_manager.hpp>
#include <Helpers.hpp>

namespace Logic
{
    TEST(user_controller, normal_usage_example)
    {
        auto dev = std::make_shared<Device>();
        auto &manager = dev->manager;
        auto &nm = dev->nm;

        /* Create extension to observe interactions between extensions */
        manager.add_ext<Observer>(Forwarder{manager.ext_forwarder}, Notyfier{0, nm}, dev);
        EXPECT_FALSE(Observer::if_change_param_job_fired());

        /* Create user controller */
        auto uc = User_controller{manager.ext_forwarder};

        /* Change parameters of connection */
        uc.set_params(
            Port_settings::Baud_rate{15200},
            Port_settings::Parity::even,
            Port_settings::Char_size{7},
            Port_settings::Stop_bits::two
        );

        /* Check if job was sent to extensions */
        EXPECT_TRUE(Observer::if_change_param_job_fired());
    }
}