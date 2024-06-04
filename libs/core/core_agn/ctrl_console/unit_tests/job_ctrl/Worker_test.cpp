#include <gtest/gtest.h>
#include <Worker.hpp>

using namespace Job_ctrl;

class Dummy_int
{
    int var;

public:
    Dummy_int(int var)
        : var{var}

    {
    }

    Dummy_int& operator-=(const Dummy_int& d) 
    {
        var -= d.var;
        
        return *this;
    }

    Dummy_int& operator+=(const Dummy_int& d) 
    {
        var += d.var;
        
        return *this;
    }

    int get_var()
    {
        return var;
    }

};

class Adder_job : public Job_identity<Adder_job>
{
public:
    Dummy_int to_add{3};
};

class Subtract_job : public Job_identity<Subtract_job>
{
public:
    Dummy_int to_subtract{2};
};

class Set_job : public Job_identity<Set_job>
{
public:
    Dummy_int to_set{33};
};

class Counter : public Worker
{
public:
    Dummy_int var{2};

    Counter()
    {
        add_handler(Job_type::Urgent, Job_policies<>::make_job_handler<Adder_job>([this](auto &to_add){ var += to_add.to_add; }));
        add_handler(Job_type::Trivial, Job_policies<>::make_job_handler<Subtract_job>([this](auto &to_subtract){ var -= to_subtract.to_subtract; }));
        add_handler(Job_type::Trivial, Job_policies<>::make_job_handler<Set_job>([this](auto &to_set){ var = std::forward<decltype(to_set.to_set)>(to_set.to_set); }));
    }

    void do_one_job()
    {
        take_job();
    }
};

TEST(job_ctrl, worker)
{
    Counter counter{};

    counter.give_job(Adder_job{});
    EXPECT_EQ(5, counter.var.get_var());

    counter.give_job(Subtract_job{});
    counter.give_job(Set_job{});
    
    counter.do_one_job();
    EXPECT_EQ(3, counter.var.get_var());

    counter.do_one_job();
    EXPECT_EQ(33, counter.var.get_var());
}