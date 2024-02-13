#include <gtest/gtest.h>
#include <Job_handler.hpp>

using namespace Job_ctrl;

class Adder_job : public Job_identity<Adder_job>
{
public:
    int to_add{3};
};

TEST(job_ctrl, job_handler)
{
    int var{};

    auto add1 = Job_policies<>::make_job_handler<Adder_job>(
        [&](auto &to_add)
        { var += to_add.to_add; });

    add1.exec(Adder_job{});

    EXPECT_EQ(3, var);
}

TEST(job_ctrl, job_handler_ptr)
{
    int var{};

    auto add = Job_policies<>::make_job_handler<Adder_job>(
        [&](auto &to_add)
        { var += to_add.to_add; });

    std::unique_ptr<Job_handler_intf> add_ptr = std::make_unique<decltype(add)>(std::forward<decltype(add)>(add));

    add_ptr->exec(Adder_job{});

    EXPECT_EQ(3, var);
}