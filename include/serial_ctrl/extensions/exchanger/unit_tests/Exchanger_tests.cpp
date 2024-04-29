#include <gtest/gtest.h>
#include <Exchanger.hpp>
#include <Process_full.hpp>
#include <Remote_record.hpp>
#include <Helpers.hpp>
#include <Get_defs.hpp>

namespace Logic
{
    using Logic_under_test = Exchanger<
        Custom_timer_impl_maker,
        Remote_sett_impl,
        Remote_side_impl,
        Serial_side_impl>;

    class Cont
    {
    public:
        /// @brief Extension manager
        std::shared_ptr<Process_full> manager = std::make_shared<Process_full>();
        /// @brief Record describing state of the device
        std::shared_ptr<Remote_record> remote_rec = std::make_shared<Remote_record>();
        /// @brief Data exchanger
        std::shared_ptr<Logic_under_test> exchanger = std::make_shared<Logic_under_test>(
            manager,
            Remote_sett_impl{},
            Remote_side_impl{},
            Serial_side_impl{},
            remote_rec);
        /// @brief Forwarding data from remote to ext
        Remote_side_conn_impl remote_c{};
        /// @brief Forwarding data from serial to ext
        Serial_side_conn_impl serial_c{};

        Cont()
        {
            /* Bind inputs to ext */
            std::apply(
                [this](auto &&...args)
                {
                    remote_c.connect(std::forward<decltype(args)>(args)...);
                },
                exchanger->get_remote_args());
            std::apply(
                [this](auto &&...args)
                {
                    serial_c.connect(std::forward<decltype(args)>(args)...);
                },
                exchanger->get_serial_args());
            /* Add previously constructed ext to manager */
            manager->add_ext(std::move(exchanger));
        }
    };

    TEST(exchanger, usual_non_error)
    {
        Cont cont{};
        auto &manager = cont.manager;
        auto &rec = cont.remote_rec;
        auto &remote_c = cont.remote_c;
        auto &serial_c = cont.serial_c;

        /* Add observer */
        manager->add_ext<Observer>(manager);

        /* Check if ext forwards data when parameters weren't set yet */
        remote_c.send("foo1");
        EXPECT_EQ(Serial_side_impl::get_last_msg(), std::vector<std::string>());
        serial_c.send("foo2");
        EXPECT_EQ(Remote_side_impl::get_last_msg(), std::vector<std::string>());

        /* Tell ext to start data exchange */
        manager->forward_job(Param_ready_notify_job{});

        /* Check if ext forwards data */
        remote_c.send("foo1");
        EXPECT_EQ(Serial_side_impl::get_last_msg(), std::vector<std::string>{"foo1"});
        serial_c.send("foo2");
        EXPECT_EQ(Remote_side_impl::get_last_msg(), std::vector<std::string>{"foo2"});
        EXPECT_EQ(Remote_side_impl::get_last_id(), Packet_defs::min_msg_num);

        /* Reset all extensions */
        manager->forward_job(Restart_job{});

        /* Check if ext forwards data when parameters weren't set yet */
        remote_c.send("foo1");
        EXPECT_EQ(Serial_side_impl::get_last_msg(), std::vector<std::string>());
        serial_c.send("foo2");
        EXPECT_EQ(Remote_side_impl::get_last_msg(), std::vector<std::string>());

        /* Tell ext to start data exchange */
        manager->forward_job(Param_ready_notify_job{});

        /* Check if ext forwards data */
        remote_c.send("foo3");
        EXPECT_EQ(Serial_side_impl::get_last_msg(), std::vector<std::string>{"foo3"});
        serial_c.send("foo4");
        EXPECT_EQ(Remote_side_impl::get_last_msg(), std::vector<std::string>{"foo4"});
        EXPECT_EQ(Remote_side_impl::get_last_id(), Packet_defs::min_msg_num + 1); // TODO set number to Packet_defs::min_msg_num after forcing reset
        serial_c.send("foo5");
        EXPECT_EQ(Remote_side_impl::get_last_msg(), std::vector<std::string>{"foo5"});
        EXPECT_EQ(Remote_side_impl::get_last_id(), Packet_defs::min_msg_num + 2);
    }
}
