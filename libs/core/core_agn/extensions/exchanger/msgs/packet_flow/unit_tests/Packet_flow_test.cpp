#include <gtest/gtest.h>
#include <Packet_flow.hpp>
#include <Packet_master.hpp>
#include <Packet_slave.hpp>
#include <Packet_defs.hpp>

using namespace Logic;

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

TEST(packet_flow, round_reload)
{
  Defs::Packet_flow_cut counter{};

  Defs::Id_t current = Defs::min_num;

  while(1)
  {
    auto num = counter.num_up();
    
    if (current > Defs::max_num)
    {
      EXPECT_EQ(num, Defs::min_num);
      EXPECT_EQ(counter.exp(), Defs::min_num + 1);

      break;
    }

    current++;
  }
}

TEST(packet_flow, ack)
{
  Defs::Packet_flow_cut counter{};

  Defs::Id_t num = Defs::min_num;

  for (
      Defs::Id_t i = 0;
      i <= 20;
      i++)
  {
    num = counter.num_up();
  }

  for (
      Defs::Id_t i = 5;
      i >= 0;
      i--)
  {
    counter.ack(num - i);
    EXPECT_EQ(counter.get_not_acked(), i);
  }
}
