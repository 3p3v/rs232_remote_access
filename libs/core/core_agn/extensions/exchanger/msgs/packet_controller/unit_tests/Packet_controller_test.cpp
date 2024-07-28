#include <gtest/gtest.h>
#include <Packet_controller.hpp>
#include <Packet_sett_final.hpp>

using namespace Logic;

namespace Defs
{
    using Id_t = Packet_sett_final::Val_t;

    using Cont = Packet_controller<Id_t>;
};

using namespace Defs;

TEST(packet_controller, no_more_free_space)
{
    Cont cont{Packet_sett_final::get()};

    for (Id_t i = 0; i < Packet_sett_final::get().max_saved; i++)
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
    Cont cont{Packet_sett_final::get()};

    for (Id_t i = 0; i < Packet_sett_final::get().max_saved; i++)
    {
        cont.create();
    }

    ASSERT_THROW(
        {
            cont.create();
        },
        std::logic_error);
}