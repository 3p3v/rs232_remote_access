#include <gtest/gtest.h>
#include <Packet_controller.hpp>
#include <Packet_defs.hpp>

using namespace Logic;

namespace Defs
{
    static constexpr auto min_num{Packet_defs::min_msg_num};
    static constexpr auto max_num{Packet_defs::max_msg_num};
    static constexpr auto max_size{30};
    using Id_t = std::decay_t<decltype(Packet_defs::max_msg_num)>;

    using Cont = Packet_controller<
        Id_t,
        min_num,
        max_num,
        max_size>;
};

TEST(packet_controller, no_more_free_space)
{
    Defs::Cont cont{};

    for (Defs::Id_t i = 0; i < Defs::max_size; i++)
    {
        auto &msg = cont.create();
        msg.unused();
    }

    ASSERT_THROW(
        {
            cont.create();
        },
        std::logic_error);
}

TEST(packet_controller, no_more_unused_space)
{
    Defs::Cont cont{};

    for (Defs::Id_t i = 0; i < Defs::max_size; i++)
    {
        cont.create();
    }

    ASSERT_THROW(
        {
            cont.create();
        },
        std::logic_error);
}