#include <gtest/gtest.h>
#include <Alpha_only.hpp>

using namespace Cmd_ctrl;

TEST(cmd_ctrl, alpha_only_policy)
{
  auto t1 = Alpha_only::validate_t("thisIsCommand");
  auto t2 = Alpha_only::validate_t("th1s1sN0tC0mmand");

  EXPECT_EQ(t1, true);
  EXPECT_EQ(t2, false);
}