#include <gtest/gtest.h>
#include <Lower_than.hpp>

using namespace Cmd_ctrl;

static constexpr int max{10};

TEST(cmd_ctrl, lower_than_policy)
{
  auto t1 = Lower_than<max>::validate_t("9");
  auto t2 = Lower_than<max>::validate_t("10");
  auto t3 = Lower_than<max>::validate_t("11");

  EXPECT_EQ(t1, true);
  EXPECT_EQ(t2, false);
  EXPECT_EQ(t3, false);
}