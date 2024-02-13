#include <gtest/gtest.h>
#include <Job.hpp>

using namespace Job_ctrl;

class Baud_rate_job : public Job_identity<Baud_rate_job>
{
public:
    unsigned int baud_rate{9600};
};

class Char_size_job : public Job_identity<Char_size_job>
{
public:
    unsigned int char_size{8};
};

TEST(job_ctrl, id_test)
{
    Baud_rate_job j1{};
    Char_size_job j2{};
    Char_size_job j3{};

    EXPECT_EQ(0, j1.get_id());
    EXPECT_EQ(1, j2.get_id());
    EXPECT_EQ(1, j3.get_id());
}