#include <gtest/gtest.h>
#include <algorithm>
#include <cmath>
#include <Mqtt_msg_cont.hpp>
#include <Packet_sett_final.hpp>

using namespace Logic;

using Id_t = Packet_sett_final::Val_t;

using Cont = Mqtt_msg_cont<Id_t>;

static void fill_with_msgs(Cont &msgs, Id_t offset, Id_t max_saved);

TEST(msgs, no_free_msgs)
{
  Cont msgs{Packet_sett_final::get()};

  unsigned int times_called;

  ASSERT_THROW(
      {
        for (times_called = 0; times_called < Packet_sett_final::get().max_saved + 1; times_called++)
        {
          auto &msg = msgs.first_free(times_called);
          msg.unused();
        }
      },
      std::logic_error);

  EXPECT_EQ(times_called, Packet_sett_final::get().max_saved);
}

TEST(msgs, messages_not_free)
{
  Cont msgs{Packet_sett_final::get()};

  const Id_t max = Packet_sett_final::get().max_saved;

  /* Fill with messages */
  fill_with_msgs(msgs, 0, max);

  /* Max msg */
  const Id_t max_msg = Packet_sett_final::get().min_msg_num + max;

  /* Check if all msgs are not free */
  for (Id_t i = Packet_sett_final::get().min_msg_num;
       i < max_msg;
       i++)
  {
    EXPECT_EQ(msgs[i].if_freed(), false);
  }
}

TEST(msgs, free_all)
{
  Cont msgs{Packet_sett_final::get()};

  const Id_t max = Packet_sett_final::get().max_saved;

  /* Fill with messages */
  fill_with_msgs(msgs, 0, max);

  /* Max msg */
  const Id_t max_msg = Packet_sett_final::get().min_msg_num + max - 1;

  /* Free all */
  msgs.free_untill(max_msg);

  /* Check if only this msgs are free */
  for (Id_t i = Packet_sett_final::get().min_msg_num;
       i < max_msg;
       i++)
  {
    EXPECT_EQ(msgs[i].if_freed(), true);
  }
}

TEST(msgs, free_some_beggining)
{
  Cont msgs{Packet_sett_final::get()};

  /* Max number of messages */
  const Id_t max_msg_n = Packet_sett_final::get().max_saved;

  /* Fill with messages */
  fill_with_msgs(msgs, 0, max_msg_n);

  /* Msg in half */
  const Id_t half_msg_n = std::ceil(static_cast<double>(max_msg_n) / 2);
  const Id_t half_msg = Packet_sett_final::get().min_msg_num + half_msg_n;

  /* Free half */
  std::cout << std::to_string(half_msg - Packet_sett_final::get().min_msg_num) << '\n';
  msgs.free_untill(half_msg);

  /* Check if right msgs were if_freed() */
  for (Id_t i = Packet_sett_final::get().min_msg_num;
       i < Packet_sett_final::get().min_msg_num + max_msg_n;
       i++)
  {
    std::cout << std::to_string(i - Packet_sett_final::get().min_msg_num) << '\n';

    if (i <= half_msg)
    {
      EXPECT_EQ(msgs[i].if_freed(), true);
    }
    else
    {
      EXPECT_EQ(msgs[i].if_freed(), false);
    }
  }
}

TEST(msgs, free_some)
{
  Cont msgs{Packet_sett_final::get()};

  /* Max number of messages */
  const Id_t max_msg_n = Packet_sett_final::get().max_saved;

  /* First msgs */
  const Id_t beg_offset = 5;
  const Id_t beg_msg_n = Packet_sett_final::get().max_msg_num - Packet_sett_final::get().min_msg_num - beg_offset;
  const Id_t beg_msg = Packet_sett_final::get().max_msg_num - beg_offset;

  /* Last msgs */
  const Id_t end_offset = 10;
  const Id_t end_msg_n = end_offset;
  const Id_t end_msg = Packet_sett_final::get().min_msg_num + end_offset;

  /* Fill with messages */
  fill_with_msgs(msgs, beg_msg_n, beg_offset);
  fill_with_msgs(msgs, 0, end_msg_n);

  /* Free some */
  const Id_t free_offset = end_offset - 5;
  const Id_t free_msg = Packet_sett_final::get().min_msg_num + free_offset;
  msgs.free_untill(free_msg);

  /* Check if right msgs were if_freed() */
  for (Id_t i = beg_msg;
       i <= end_msg;
       i++)
  {
    std::cout << std::to_string(i - Packet_sett_final::get().min_msg_num) << '\n';

    if (i >= beg_msg || i <= free_msg)
    {
      EXPECT_EQ(msgs[i].if_freed(), true);
    }
    else
    {
      EXPECT_EQ(msgs[i].if_freed(), false);
    }
  }
}

TEST(msgs, choose_oldest)
{
  Cont msgs{Packet_sett_final::get()};

  /* Max number of messages */
  const Id_t max_msg_n = Packet_sett_final::get().max_saved;

  /* First msgs */
  const Id_t beg_offset = 5;
  const Id_t beg_msg_n = Packet_sett_final::get().max_msg_num - Packet_sett_final::get().min_msg_num - beg_offset;
  const Id_t beg_msg = Packet_sett_final::get().max_msg_num - beg_offset;
  std::cout << "MSG_NUM: " << std::to_string(beg_msg) << '\n';

  /* Last msgs */
  const Id_t end_msg_n = Packet_sett_final::get().max_saved - beg_offset;
  const Id_t end_msg = Packet_sett_final::get().min_msg_num + end_msg_n;

  /* Fill with messages */
  fill_with_msgs(msgs, beg_msg_n, beg_offset);
  fill_with_msgs(msgs, 0, end_msg_n);

  /* Free some */
  ASSERT_THROW(
      {
        msgs.first_free(end_msg + 1);
      },
      std::logic_error);

  msgs.oldest(end_msg + 1);

  ASSERT_THROW(
      {
        msgs[beg_msg];
      },
      std::logic_error);
}

TEST(msgs, no_more_unused_msgs)
{
  Cont msgs{Packet_sett_final::get()};

  auto end_msg = Packet_sett_final::get().min_msg_num + Packet_sett_final::get().max_saved;

  for (Id_t i = (Packet_sett_final::get().min_msg_num);
       i < end_msg;
       i++)
  {
    auto &msg = msgs.first_free(i);
  }

  ASSERT_THROW(
      {
        msgs.first_free(end_msg + 1);
      },
      std::logic_error);

  ASSERT_THROW(
      {
        msgs.oldest(end_msg + 1);
      },
      std::logic_error);
}

/* Helper functions */
static void fill_with_msgs(Cont &msgs, Id_t offset, Id_t max_saved)
{
  for (Id_t i = (Packet_sett_final::get().min_msg_num + offset);
       i < (Packet_sett_final::get().min_msg_num + offset) + max_saved;
       i++)
  {
    auto &msg = msgs.first_free(i);
    msg.unused();
  }
}