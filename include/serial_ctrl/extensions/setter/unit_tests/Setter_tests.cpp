#include <gtest/gtest.h>
#include <Setter.hpp>
#include <Process_full.hpp>
// #include <Hi_defs.hpp>
#include <Helpers.hpp>
#include <Get_defs.hpp>
#include <Restart_job.hpp>
#include <Notyfication_manager.hpp>
#include <Notyfier.hpp>

namespace Logic
{
    using Logic_under_test = Setter<Custom_timer_impl_maker, Remote_sett_impl, Serial_sett_impl>;

    TEST(setter, usual_non_error)
    {
        auto manager = Process_full{};
        auto nm = Notyfication_manager{};
        auto rec = Remote_record{Remote_conf_port::Configurable};
        auto dev = std::make_shared<Device>();

        /* Create logic extension and add it to manager */
        auto rs = Remote_sett_impl{};
        auto ss = Serial_sett_impl{};
        manager.add_ext<Logic_under_test>(Forwarder{manager.ext_forwarder}, Notyfier{0, nm}, dev, rs, ss, rec);
        manager.add_ext<Observer>(Forwarder{manager.ext_forwarder}, Notyfier{1, nm}, dev);

        /* Initial */
        EXPECT_TRUE(Remote_sett_impl::get_last_msg_s().empty());
        EXPECT_TRUE(Remote_sett_impl::get_last_msg_i().empty());
        EXPECT_NE(rec.port_settings.baud_rate, Serial_sett_impl::baud_rate);
        EXPECT_NE(rec.port_settings.parity, Serial_sett_impl::parity);
        EXPECT_NE(rec.port_settings.char_size, Serial_sett_impl::char_size);
        EXPECT_NE(rec.port_settings.stop_bits, Serial_sett_impl::stop_bits);

        /* Send signal to change params */
        manager.ext_forwarder.forward_job(Get_set_param_job{});
        EXPECT_EQ(Observer::if_get_set_param_fired(), true);
        EXPECT_EQ(Observer::if_param_ready_fired(), false);
        EXPECT_NE(rec.port_settings.baud_rate, Serial_sett_impl::baud_rate);
        EXPECT_NE(rec.port_settings.parity, Serial_sett_impl::parity);
        EXPECT_NE(rec.port_settings.char_size, Serial_sett_impl::char_size);
        EXPECT_NE(rec.port_settings.stop_bits, Serial_sett_impl::stop_bits);
        auto sent_cmds = std::vector<std::string>{
            std::string{Set_defs::set_baud_rate_s} + ' ' + Set_defs::baud_rate_trans(rec.port_settings.baud_rate),
            std::string{Set_defs::set_parity_s} + ' ' + Set_defs::parity_trans(rec.port_settings.parity),
            std::string{Set_defs::set_char_size_s} + ' ' + Set_defs::char_size_trans(rec.port_settings.char_size),
            std::string{Set_defs::set_stop_bits_s} + ' ' + Set_defs::stop_bits_trans(rec.port_settings.stop_bits)};
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s(), sent_cmds);

        /* Send command confirming change */
        auto sh = std::string{Get_defs::get_baud_rate_s} + ' ' + std::to_string(rec.port_settings.baud_rate) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_parity_s} + ' ' + Set_defs::parity_trans(rec.port_settings.parity) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_char_size_s} + ' ' + std::to_string(rec.port_settings.char_size) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_stop_bits_s} + ' ' + Set_defs::stop_bits_trans(rec.port_settings.stop_bits) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        EXPECT_TRUE(Observer::if_param_ready_fired());
        EXPECT_EQ(rec.port_settings.baud_rate, Serial_sett_impl::baud_rate);
        EXPECT_EQ(rec.port_settings.parity, Serial_sett_impl::parity);
        EXPECT_EQ(rec.port_settings.char_size, Serial_sett_impl::char_size);
        EXPECT_EQ(rec.port_settings.stop_bits, Serial_sett_impl::stop_bits);
    }

    TEST(setter, settable_wrong_sequence)
    {
        auto manager = Process_full{};
        auto nm = Notyfication_manager{};
        auto rec = Remote_record{Remote_conf_port::Configurable};
        auto dev = std::make_shared<Device>();

        /* Create logic extension and add it to manager */
        auto rs = Remote_sett_impl{};
        auto ss = Serial_sett_impl{};
        manager.add_ext<Logic_under_test>(Forwarder{manager.ext_forwarder}, Notyfier{0, nm}, dev, rs, ss, rec);
        manager.add_ext<Observer>(Forwarder{manager.ext_forwarder}, Notyfier{1, nm}, dev);

        /* Send commands confirming successfull change of parameters parameters */
        auto sh = std::string{Get_defs::get_baud_rate_s} + ' ' + std::to_string(rec.port_settings.baud_rate) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_parity_s} + ' ' + Set_defs::parity_trans(rec.port_settings.parity) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_char_size_s} + ' ' + std::to_string(rec.port_settings.char_size) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_stop_bits_s} + ' ' + Set_defs::stop_bits_trans(rec.port_settings.stop_bits) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        /* Changes should be ignored */
        EXPECT_FALSE(Observer::if_param_ready_fired());
        EXPECT_NE(rec.port_settings.baud_rate, Serial_sett_impl::baud_rate);
        EXPECT_NE(rec.port_settings.parity, Serial_sett_impl::parity);
        EXPECT_NE(rec.port_settings.char_size, Serial_sett_impl::char_size);
        EXPECT_NE(rec.port_settings.stop_bits, Serial_sett_impl::stop_bits);
    }

    TEST(setter, restart_non_error)
    {
        auto manager = Process_full{};
        auto nm = Notyfication_manager{};
        auto rec = Remote_record{Remote_conf_port::Configurable};
        auto dev = std::make_shared<Device>();

        /* Create logic extension and add it to manager */
        auto rs = Remote_sett_impl{};
        auto ss = Serial_sett_impl{};
        manager.add_ext<Logic_under_test>(Forwarder{manager.ext_forwarder}, Notyfier{0, nm}, dev, rs, ss, rec);
        manager.add_ext<Observer>(Forwarder{manager.ext_forwarder}, Notyfier{1, nm}, dev);

        /* Send signal to change params */
        manager.ext_forwarder.forward_job(Get_set_param_job{});
        Remote_sett_impl::get_last_msg_s();

        /* Restart modules */
        manager.ext_forwarder.forward_job(Restart_job{});
        EXPECT_FALSE(rec.record_active);

        /* Send signal to change params */
        manager.ext_forwarder.forward_job(Get_set_param_job{});
        EXPECT_TRUE(rec.record_active);

        /* Send command confirming change */
        auto sh = std::string{Get_defs::get_baud_rate_s} + ' ' + std::to_string(rec.port_settings.baud_rate) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_parity_s} + ' ' + Set_defs::parity_trans(rec.port_settings.parity) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_char_size_s} + ' ' + std::to_string(rec.port_settings.char_size) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        sh = std::string{Get_defs::get_stop_bits_s} + ' ' + Set_defs::stop_bits_trans(rec.port_settings.stop_bits) + '\n';
        manager.cmd_exec.exec(std::begin(sh), std::end(sh));
        EXPECT_TRUE(Observer::if_param_ready_fired());
        EXPECT_EQ(rec.port_settings.baud_rate, Serial_sett_impl::baud_rate);
        EXPECT_EQ(rec.port_settings.parity, Serial_sett_impl::parity);
        EXPECT_EQ(rec.port_settings.char_size, Serial_sett_impl::char_size);
        EXPECT_EQ(rec.port_settings.stop_bits, Serial_sett_impl::stop_bits);
        auto sent_cmds = std::vector<std::string>{
            std::string{Set_defs::set_baud_rate_s} + ' ' + Set_defs::baud_rate_trans(rec.port_settings.baud_rate),
            std::string{Set_defs::set_parity_s} + ' ' + Set_defs::parity_trans(rec.port_settings.parity),
            std::string{Set_defs::set_char_size_s} + ' ' + Set_defs::char_size_trans(rec.port_settings.char_size),
            std::string{Set_defs::set_stop_bits_s} + ' ' + Set_defs::stop_bits_trans(rec.port_settings.stop_bits)};
        EXPECT_EQ(Remote_sett_impl::get_last_msg_s(), sent_cmds);
    }
}