#include <gtest/gtest.h>
#include <memory>
#include <Manager.hpp>

using namespace Job_ctrl;

static std::shared_ptr<int> observed_val{std::make_shared<int>(21)};

class Adder_job : public Job_identity<Adder_job>
{
public:
    int to_add{3};

    Adder_job(int to_add)
        : to_add{to_add}
    {
    }
};

class Subtract_job : public Job_identity<Subtract_job>
{
public:
    int to_subtract{2};

    Subtract_job(int to_subtract)
        : to_subtract{to_subtract}
    {
    }
};

class Set_job : public Job_identity<Set_job>
{
public:
    int to_set{33};

    Set_job(int to_set)
        : to_set{to_set}
    {
    }
};

class Counter : public Worker
{
    std::shared_ptr<int> val;

public:
    Counter(std::shared_ptr<int> start_val)
        : val{start_val}
    {
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Adder_job>(
                [this](auto &to_add)
                { *val += to_add.to_add; }));
        add_handler(
            Job_type::Trivial,
            Job_policies<>::make_job_handler<Subtract_job>(
                [this](auto &to_subtract)
                { *val -= to_subtract.to_subtract; }));
        add_handler(
            Job_type::Trivial,
            Job_policies<>::make_job_handler<Set_job>(
                [this](auto &to_set)
                { *val = std::forward<decltype(to_set.to_set)>(to_set.to_set); }));
    }

    void do_one_job()
    {
        take_job();
    }
};

TEST(cmd_ctrl, manager)
{
    auto counter = Counter{observed_val};
    Manager man{};
    man.add_worker(counter);

    // Job should fire immediately
    man.forward_job(Adder_job{3});
    EXPECT_EQ(24, *observed_val);

    // Should fire after manager user decides
    man.forward_job(Subtract_job{3});
    EXPECT_EQ(24, *observed_val);
    counter.do_one_job();
    EXPECT_EQ(21, *observed_val);

    // Should fire after manager user decides
    man.forward_job(Set_job{56});
    EXPECT_EQ(21, *observed_val);
    counter.do_one_job();
    EXPECT_EQ(56, *observed_val);
}