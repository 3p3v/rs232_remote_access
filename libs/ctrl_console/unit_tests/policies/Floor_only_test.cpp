#include <gtest/gtest.h>
#include <Floor_only.hpp>

using namespace Cmd_ctrl;

TEST(cmd_ctrl, floor_only_policy)
{
  auto t1 = Floor_only::validate_t("______");
  auto t2 = Floor_only::validate_t("not_floor");

  EXPECT_EQ(t1, true);
  EXPECT_EQ(t2, false);
}