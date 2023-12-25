#include <gtest/gtest.h>
#include <Packet_controller.hpp>
#include <Packet_defs.hpp>

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
    bool error{false};
    auto current = Defs::min_num;

    while (1)
    {
        auto &msg = cont.create([&error](auto &)
                                { error = true; });
        msg.unused();

        EXPECT_EQ(msg.id(), current++);

        if (!error)
        {
            EXPECT_EQ(error, false);
        }
        else
        {
            EXPECT_EQ(error, true);

            break;
        }
    }
}

TEST(packet_controller, no_more_unused_space)
{
    Defs::Cont cont{};
    bool error{false};

    for (Defs::Id_t i = 0; i < Defs::max_size; i++)
    {
        cont.create([&error](auto &)
                    { error = true; });
    }

    EXPECT_EQ(error, false);

    ASSERT_THROW(
        {
            cont.create([&error](auto &)
                        { error = true; });
        },
        std::logic_error);

    EXPECT_EQ(error, false);
}