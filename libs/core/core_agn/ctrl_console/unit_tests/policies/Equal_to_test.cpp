#include <gtest/gtest.h>
#include <Equal_to.hpp>

using namespace Cmd_ctrl;

static constexpr int num{10};

TEST(cmd_ctrl, equal_to_policy)
{
  auto t1 = Equal_to<decltype(num), num>::validate_t("9");
  auto t2 = Equal_to<decltype(num), num>::validate_t("10");
  auto t3 = Equal_to<decltype(num), num>::validate_t("11");

  EXPECT_EQ(t1, false);
  EXPECT_EQ(t2, true);
  EXPECT_EQ(t3, false);
}