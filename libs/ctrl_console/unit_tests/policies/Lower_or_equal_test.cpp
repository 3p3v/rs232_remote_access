#include <gtest/gtest.h>
#include <Lower_or_equal.hpp>

using namespace Cmd_ctrl;

static constexpr int max{10};

TEST(cmd_ctrl, lower_or_equal_policy)
{
  auto t1 = Lower_or_equal<decltype(max), max>::validate_t("9");
  auto t2 = Lower_or_equal<decltype(max), max>::validate_t("10");
  auto t3 = Lower_or_equal<decltype(max), max>::validate_t("11");

  EXPECT_EQ(t1, true);
  EXPECT_EQ(t2, true);
  EXPECT_EQ(t3, false);
}