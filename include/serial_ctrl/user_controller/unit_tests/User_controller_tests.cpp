#include <gtest/gtest.h>
#include <User_controller.hpp>
#include <Process_full.hpp>
#include <Remote_full.hpp>
#include <Helpers.hpp>

namespace Logic
{
    TEST(user_controller, normal_usage_example)
    {
        auto manager = std::make_shared<Process_full>();
        auto rec = std::make_shared<Remote_full>("T0", "COM0", Remote_conf_port::Configurable);

        /* Create extension to observe interactions between extensions */
        manager->add_ext<Observer>(manager);
        EXPECT_FALSE(Observer::if_change_param_job_fired());

        /* Create user controller */
        auto uc = User_controller{manager};

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