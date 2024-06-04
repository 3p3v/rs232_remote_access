#include <gtest/gtest.h>
#include <memory>
#include <Device_holder.hpp>
#include <Remote_dummy.hpp>
#include <Serial_dummy.hpp>
#include <Timer_dummy.hpp>
#include <Remote_dev.hpp>
#include <Notyfication_manager.hpp>
#include <Device.hpp>

namespace Logic
{
    using Dut = Device<
        Timer_dummy_maker,
        Remote_dummy,
        Serial_dummy>;

    TEST(device, init_test)
    {
        auto nm = Notyfication_manager{};

        Device_holder dev = Device_holder::make_device<Dut>(
            Notyfier{0, nm},
            Remote_dev{},
            Remote_dummy{},
            Serial_dummy{});

        // EXPECT_EQ(rec->port_settings.stop_bits, Serial_sett_impl::stop_bits);
    }
}