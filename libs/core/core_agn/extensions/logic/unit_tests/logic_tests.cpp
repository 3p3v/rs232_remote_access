#include <gtest/gtest.h>
#include <Logic.hpp>
#include <Process_full.hpp>
#include <Hi_defs.hpp>
#include <Helpers.hpp>
#include <Notyfication_manager.hpp>
#include <Notyfier.hpp>

namespace Logic
{
    using Logic_under_test = Logic<Custom_timer_impl_maker, Remote_sett_impl>;

    TEST(logic, usual_non_error)
    {
        auto manager = Process_full{};
        auto nm = Notyfication_manager{};
        auto rec = Remote_status_record{Remote_conf_port::Configurable};
        auto dev = std::make_shared<Device>();

        /* Create logic extension and add it to manager */
        auto rs = Remote_sett_impl{};
        manager.add_ext<Logic_under_test>(Forwarder{manager.ext_forwarder}, Notyfier{0, nm}, dev, Mqtt_settings<Remote_sett_impl>{rs}, rec);
        manager.add_ext<Observer>(Forwarder{manager.ext_forwarder}, Notyfier{1, nm}, dev);

        /* Initial */
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s().empty(), true);
        EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
        EXPECT_EQ(rec.status, Remote_status::Not_connected);

        /* Start communication */
        manager.ext_forwarder.forward_job(Start_job{});
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s(), Hi_defs::master_hi_s.data());
        EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
        EXPECT_EQ(rec.status, Remote_status::Not_connected);

        /* Remote sends hi */
        auto sh = std::string{Hi_defs::slave_hi_s} + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        EXPECT_EQ(rec.status, Remote_status::Establishing_parameters);
        EXPECT_EQ(Observer::if_get_set_param_fired(), true);

        /* Send signal that parameters were established */
        Observer::param_ready_notify_job();
        EXPECT_EQ(rec.status, Remote_status::Data_exchange);
    }

    TEST(logic, non_configurable_non_error)
    {
        auto manager = Process_full{};
        auto nm = Notyfication_manager{};
        auto rec = Remote_status_record{Remote_conf_port::Non_configurable};
        auto dev = std::make_shared<Device>();

        /* Create logic extension and add it to manager */
        auto rs = Remote_sett_impl{};
        manager.add_ext<Logic_under_test>(Forwarder{manager.ext_forwarder}, Notyfier{0, nm}, dev, Mqtt_settings<Remote_sett_impl>{rs}, rec);
        manager.add_ext<Observer>(Forwarder{manager.ext_forwarder}, Notyfier{1, nm}, dev);

        /* Initial */
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s().empty(), true);
        EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
        EXPECT_EQ(rec.status, Remote_status::Not_connected);

        /* Start communication */
        manager.ext_forwarder.forward_job(Start_job{});
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s().empty(), true);
        EXPECT_EQ(Remote_sett_impl::get_last_msg_i(), Hi_defs::master_keep_alive_s.data());
        EXPECT_EQ(rec.status, Remote_status::Not_connected);

        /* Remote sends hi */
        auto sh = std::string{Hi_defs::slave_keep_alive_s} + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        EXPECT_EQ(rec.status, Remote_status::Establishing_parameters);
        EXPECT_EQ(Observer::if_get_set_param_fired(), true);

        /* Send signal that parameters were established */
        Observer::param_ready_notify_job();
        EXPECT_EQ(rec.status, Remote_status::Data_exchange);
    }

    TEST(logic, disconnect)
    {
        auto manager = Process_full{};
        auto nm = Notyfication_manager{};
        auto rec = Remote_status_record{Remote_conf_port::Configurable};
        auto dev = std::make_shared<Device>();

        /* Create logic extension and add it to manager */
        auto rs = Remote_sett_impl{};
        manager.add_ext<Logic_under_test>(Forwarder{manager.ext_forwarder}, Notyfier{0, nm}, dev, Mqtt_settings<Remote_sett_impl>{rs}, rec);
        manager.add_ext<Observer>(Forwarder{manager.ext_forwarder}, Notyfier{1, nm}, dev);

        /* Initial */
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s().empty(), true);
        EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
        EXPECT_EQ(rec.status, Remote_status::Not_connected);

        /* Start communication */
        manager.ext_forwarder.forward_job(Start_job{});
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s(), Hi_defs::master_hi_s.data());
        EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
        EXPECT_EQ(rec.status, Remote_status::Not_connected);

        /* Remote sends hi */
        auto sh = std::string{Hi_defs::slave_hi_s} + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        EXPECT_EQ(rec.status, Remote_status::Establishing_parameters);
        EXPECT_EQ(Observer::if_get_set_param_fired(), true);

        /* Got restart */
        Observer::restart_job_();
        EXPECT_EQ(rec.status, Remote_status::Disconnected);
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s(), Hi_defs::master_hi_s.data());
    }
}