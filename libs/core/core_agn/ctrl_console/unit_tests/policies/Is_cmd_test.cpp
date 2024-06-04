#include <gtest/gtest.h>
#include <Is_cmd.hpp>

using namespace Cmd_ctrl;

TEST(cmd_ctrl, is_cmd_policy)
{
  auto t1 = Is_cmd::validate_t("THIS_IS_COMAND");
  auto t2 = Is_cmd::validate_t("tHISIs_NOT_COMMAND");

  EXPECT_EQ(t1, true);
  EXPECT_EQ(t2, false);
}