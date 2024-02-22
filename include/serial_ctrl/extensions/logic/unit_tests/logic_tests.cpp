#include <gtest/gtest.h>
#include <Logic.hpp>
#include <Basic_timer.hpp>

using namespace Logic;

class Remote_sett_impl;

template <typename Callb>
class Custom_timer_impl;

using Logic_under_test = Logic<Custom_timer_impl, Remote_sett_impl>;

TEST(logic, recognise_command)
{
    auto manager = std::make_shared<Process_manager>();
    auto rec = std::make_shared<Remote_status_record>(Remote_conf_port::Configurable);

    Logic_under_test log{manager, rec, Remote_sett_impl{}};

    /* Start communication */
    log.give_job(Start_job{});
    EXPECT_EQ()


    log.give_job(Start_job{});

    EXPECT_EQ(val, new_val);
    EXPECT_STREQ(rec_arg.data(), argument.data());
}

class Remote_sett_impl
{
    static std::string last_msg_s{};
    static std::string last_msg_i{};

public:
    auto get_last_msg_s()
    {
        auto last_ = last_msg_s;
        last_msg.clear();
        return last_;
    }

    auto get_last_msg_i()
    {
        auto last_ = last_msg_i;
        last_msg.clear();
        return last_;
    }

    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    inline void write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        last_msg_i = msg + ' ' + arg;
    }

    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        last_msg_i = msg;
    }

    template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
    inline void write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        last_msg_s = msg + ' ' + arg;
    }

    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        last_msg_s = msg;
    }
}

template <typename Callb>
class Custom_timer_impl : public Basic_timer
{
    /* Callback */
    Callb callb;

public:
    void start() override
    {

    }

    void stop() override
    {
        
    }

    Custom_timer(Callb &&callb);
};