#include <gtest/gtest.h>
#include <Or.hpp>
#include <Alpha_only.hpp>
#include <Floor_only.hpp>

using namespace Cmd_ctrl;

using Alpha_or_floor = Or<Alpha_only, Floor_only>;

TEST(cmd_ctrl, or_policy)
{
  auto t1 = Alpha_or_floor::validate_t("_");
  auto t2 = Alpha_or_floor::validate_t("a");
  auto t3 = Alpha_or_floor::validate_t("5");

  EXPECT_EQ(t1, true);
  EXPECT_EQ(t2, true);
  EXPECT_EQ(t3, false);
}