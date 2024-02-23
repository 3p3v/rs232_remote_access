#include <gtest/gtest.h>
#include <Logic.hpp>
#include <Process_full.hpp>
#include <Hi_defs.hpp>

namespace Logic
{

    class Remote_sett_impl;

    template <typename Callb>
    class Custom_timer_impl;

    class Custom_timer_impl_maker;

    class Observer;

    using Logic_under_test = Logic<Custom_timer_impl_maker, Remote_sett_impl>;

    TEST(logic, usual_non_error)
    {
        auto manager = std::make_shared<Process_full>();
        auto rec = std::make_shared<Remote_status_record>(Remote_conf_port::Configurable);

        /* Create logic extension and add it to manager */
        manager->add_ext<Logic_under_test>(manager, rec, Remote_sett_impl{});

        /* Initial */
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s().empty(), true);
        EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
        EXPECT_EQ(rec->status, Remote_status::Not_connected);

        /* Start communication */
        manager->forward_job(Start_job{});
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s(), Hi_defs::master_hi_s.data());
        EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
        EXPECT_EQ(rec->status, Remote_status::Not_connected);

        /* Remote sends hi */
        auto sh = std::string{Hi_defs::slave_hi_s} + '\n';
        manager->exec(std::begin(sh), std::end(sh));
        EXPECT_EQ(rec->status, Remote_status::Establishing_parameters);
        EXPECT_EQ(Observer::if_get_set_param_fired(), true);

        /* Send signal that parameters were established */
        Observer::param_ready_notify_job();
        EXPECT_EQ(rec->status, Remote_status::Data_exchange);
    }

    
}