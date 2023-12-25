#include <gtest/gtest.h>
#include <Bigger_or_equal.hpp>

using namespace Cmd_ctrl;

static constexpr int min{10};

TEST(cmd_ctrl, bigger_or_equal_policy)
{
  auto t1 = Bigger_or_equal<min>::validate_t("9");
  auto t2 = Bigger_or_equal<min>::validate_t("10");
  auto t3 = Bigger_or_equal<min>::validate_t("11");

  EXPECT_EQ(t1, false);
  EXPECT_EQ(t2, true);
  EXPECT_EQ(t3, true);
}