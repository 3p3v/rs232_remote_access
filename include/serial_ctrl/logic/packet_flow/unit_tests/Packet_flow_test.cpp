#include <gtest/gtest.h>
#include <Packet_flow.hpp>
#include <Packet_master.hpp>
#include <Packet_slave.hpp>
#include <Packet_defs.hpp>

namespace Defs
{
  static constexpr auto min_num{Packet_defs::min_msg_num};
  static constexpr auto max_num{Packet_defs::max_msg_num};
  using Id_t = std::decay_t<decltype(Packet_defs::max_msg_num)>;

  using Counter = Packet_flow<
      Id_t,
      min_num,
      max_num>;

  class Packet_flow_cut : public Defs::Counter
  {
  public:
    Defs::Id_t num_up()
    {
      return num_up_();
    }
  };
}

TEST(packet_flow, no_free_msgs)
{
  Defs::Packet_flow_cut msgs{};

  for (
      Defs::Id_t i = 0;
      i <= Defs::max_num - Defs::min_num + 1;
      i++)
  {
    if (i == (Defs::max_num - Defs::min_num + 1))
    {
      EXPECT_EQ(i, Defs::min_num);
    }
  }
}
