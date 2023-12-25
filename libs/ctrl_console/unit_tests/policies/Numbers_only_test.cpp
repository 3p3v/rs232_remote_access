#include <gtest/gtest.h>
#include <Numbers_only.hpp>

using namespace Cmd_ctrl;

TEST(cmd_ctrl, numbers_only_policy)
{
  auto t1 = Numbers_only::validate_t("n0t_num83rs");
  auto t2 = Numbers_only::validate_t("1234567890");

  EXPECT_EQ(t1, false);
  EXPECT_EQ(t2, true);
}