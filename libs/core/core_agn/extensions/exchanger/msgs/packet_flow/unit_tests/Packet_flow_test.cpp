#include <gtest/gtest.h>
#include <Packet_flow.hpp>
#include <Packet_master.hpp>
#include <Packet_slave.hpp>
#include <Packet_sett_final.hpp>

using namespace Logic;

namespace Defs
{
  using Id_t = Packet_sett_final::Val_t;

  using Counter = Packet_flow<Id_t>;

  class Packet_flow_cut : public Counter
  {
    using Counter::Packet_flow;
  public:
    Id_t num_up()
    {
      return num_up_();
    }
  };
}

using namespace Defs;

TEST(packet_flow, round_reload)
{
  Packet_flow_cut counter{Packet_sett_final::get()};

  Id_t current = Packet_sett_final::get().min_msg_num;

  while(1)
  {
    auto num = counter.num_up();
    
    if (current > Packet_sett_final::get().max_msg_num)
    {
      EXPECT_EQ(num, Packet_sett_final::get().min_msg_num);
      EXPECT_EQ(counter.exp(), Packet_sett_final::get().min_msg_num + 1);

      break;
    }

    current++;
  }
}

TEST(packet_flow, ack)
{
  Packet_flow_cut counter{Packet_sett_final::get()};

  Id_t num = Packet_sett_final::get().min_msg_num;

  for (
      Id_t i = 0;
      i <= Packet_sett_final::get().max_saved / 2;
      i++)
  {
    num = counter.num_up();
  }

  for (
      Id_t i = 5;
      i >= 0;
      i--)
  {
    counter.ack(num - i);
    EXPECT_EQ(counter.get_not_acked(), i);
  }
}
