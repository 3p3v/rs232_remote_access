#include <gtest/gtest.h>
#include <Base_ctrl_console.hpp>
#include <Exec.hpp>
#include <string>
#include <Lower_or_equal.hpp>
#include <No_arg.hpp>

using Handle_t = Cmd_ctrl::Exec<int &>::Param<std::string &&>;
using Ctrl_console = Cmd_ctrl::Base_ctrl_console<Handle_t, Cmd_ctrl::Endl_opt::without>;

TEST(cmd_ctrl, recognise_command)
{
    Ctrl_console console{};

    std::string argument{"test_arg"};
    std::string input{"test_cmd " + argument};
    constexpr int new_val{8};

    std::string rec_arg{};
    console.add_cmd(
        std::string("test_cmd"),
        Ctrl_console::Handle::Policies<>::Dyn_handle(
            [&argument, &new_val, &rec_arg](int &val, std::string &&arg)
            {
                val = new_val;
                rec_arg = std::move(arg);
            }));

    int val{6};

    console.exec(input.begin(), input.end(), val);

    EXPECT_EQ(val, new_val);
    EXPECT_STREQ(rec_arg.data(), argument.data());
}

TEST(cmd_ctrl, same_name_cmds)
{
    Ctrl_console console{};

    std::string cmd_name{"test_cmd"};
    console.add_cmd(
        std::string(cmd_name),
        Ctrl_console::Handle::Policies<>::Dyn_handle(
            [](int &val, std::string &&arg) {

            }));

    ASSERT_THROW(
        {
            console.add_cmd(
                std::string(cmd_name),
                Ctrl_console::Handle::Policies<>::Dyn_handle(
                    [](int &val, std::string &&arg) {

                    }));
        },
        std::logic_error);
}

TEST(cmd_ctrl, apply_policy)
{
    Ctrl_console console{};

    std::string argument_1{"58"};
    std::string argument_2{"59"};
    std::string input_1{"test_cmd " + argument_1};
    std::string input_2{"test_cmd " + argument_2};
    constexpr int max_val{58};

    std::string rec_arg{};
    console.add_cmd(
        std::string("test_cmd"),
        Ctrl_console::Handle::Policies<Cmd_ctrl::Lower_or_equal<decltype(max_val), max_val>>::Dyn_handle(
            [&rec_arg](int &val, std::string &&arg)
            {
                rec_arg = std::move(arg);
            }));

    int val{6};

    /* OK */
    console.exec(input_1.begin(), input_1.end(), val);
    EXPECT_STREQ(rec_arg.data(), argument_1.data());

    ASSERT_THROW(
        {
            /* ERROR */
            console.exec(input_2.begin(), input_2.end(), val);
        },
        std::logic_error);
}

TEST(cmd_ctrl, no_argument)
{
    Ctrl_console console{};

    int dummy_val{};

    constexpr int exp{9};
    constexpr int start{0};
    int var{start};
    
    std::string ok_cmd{"test_cmd"};
    std::string ec_cmd{"test_cmd 1"};

    std::string rec_arg{};
    console.add_cmd(
        std::string("test_cmd"),
        Ctrl_console::Handle::Policies<Cmd_ctrl::No_arg>::Dyn_handle(
            [&var, exp](int &val, std::string &&arg)
            {
                /**/
                var = exp;
            }));

    /* OK */
    EXPECT_EQ(var, start);
    console.exec(ok_cmd.begin(), ok_cmd.end(), dummy_val);
    EXPECT_EQ(var, exp);

    ASSERT_THROW(
        {
            /* ERROR */
            console.exec(ec_cmd.begin(), ec_cmd.end(), dummy_val);
        },
        std::logic_error);
}