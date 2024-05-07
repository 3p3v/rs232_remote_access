// #include <gtest/gtest.h>
#include <Logic.hpp>
#include <Process_full.hpp>
#include <Hi_defs.hpp>
#include <./Helpers.hpp>
#include <iostream>

namespace Logic
{
    using Logic_under_test = Logic::Logic<Custom_timer_impl_maker, Remote_sett_impl>;
}

using namespace Logic;

auto get_stat_name(const Remote_status &stat)
{
    switch (stat)
    {
    case Remote_status::Not_connected:
        return "Remote_status::Not_connected";
        break;
    case Remote_status::Establishing_parameters:
        return "Remote_status::Establishing_parameters";
        break;
    case Remote_status::Data_exchange:
        return "Remote_status::Data_exchange";
        break;
    case Remote_status::Disconnected:
        return "Remote_status::Disconnected";
        break;
    default:
        return "???";
        break;
    }
}

int main()
{
    // auto manager = std::make_shared<Process_full>();
    // auto rec = std::make_shared<Remote_status_record>(Remote_conf_port::Non_configurable);

    // /* Create logic extension and add it to manager */
    // manager->add_ext<Logic_under_test>(manager, Remote_sett_impl{}, rec);
    // manager->add_ext<Observer>(manager);

    // /* Initial */
    // // EXPECT_EQ(Remote_sett_impl::get_last_msg_s().empty(), true);
    // // EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
    // // EXPECT_EQ(rec->status, Remote_status::Not_connected);

    // /* Start communication */
    // manager->forward_job(Start_job{});
    // std::cout << Remote_sett_impl::get_last_msg_s() << '\n';
    // std::cout << Remote_sett_impl::get_last_msg_i() << '\n';
    // // EXPECT_EQ(Remote_sett_impl::get_last_msg_s(), Hi_defs::master_hi_s.data());
    // // EXPECT_EQ(Remote_sett_impl::get_last_msg_i().empty(), true);
    // // EXPECT_EQ(rec->status, Remote_status::Not_connected);

    // /* Remote sends hi */
    // auto sh = std::string{Hi_defs::slave_hi_s} + '\n';
    // manager->exec(std::begin(sh), std::end(sh));
    // std::cout << get_stat_name(rec->status) << '\n';
    // std::cout << Observer::if_get_set_param_fired() << '\n';
    // // EXPECT_EQ(rec->status, Remote_status::Establishing_parameters);
    // // EXPECT_EQ(Observer::if_get_set_param_fired(), true);

    // /* Send signal that parameters were established */
    // Observer::restart_job_();
    // std::cout << get_stat_name(rec->status) << '\n';
    // // EXPECT_EQ(rec->status, Remote_status::Data_exchange);

    return 0;
}
