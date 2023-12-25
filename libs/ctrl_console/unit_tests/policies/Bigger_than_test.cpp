#include <gtest/gtest.h>
#include <Bigger_than.hpp>

using namespace Cmd_ctrl;

static constexpr int min{10};

TEST(cmd_ctrl, bigger_than_policy)
{
  auto t1 = Bigger_than<min>::validate_t("9");
  auto t2 = Bigger_than<min>::validate_t("10");
  auto t3 = Bigger_than<min>::validate_t("11");

  EXPECT_EQ(t1, false);
  EXPECT_EQ(t2, false);
  EXPECT_EQ(t3, true);
}