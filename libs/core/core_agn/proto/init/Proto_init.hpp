#pragma once

#include <memory>
/* Proto modules */
#include <Greeter.hpp>
#include <Exchanger.hpp>
/* Commands */
#include <Keep_alive.hpp>
#include <Master_hi.hpp>
#include <Slave_hi.hpp>
#include <Set_baud_rate.hpp>
#include <Set_char_size.hpp>
#include <Set_parity.hpp>
#include <Set_stop_bits.hpp>
#include <Get_baud_rate.hpp>
#include <Get_char_size.hpp>
#include <Get_parity.hpp>
#include <Get_stop_bits.hpp>
#include <Packet_ack.hpp>
#include <No_number.hpp>
#include <Invalid_number.hpp>
#include <Disconnect_detect.hpp>

namespace Logic
{
    class Proto_init
    {
        template <typename Device_t>
        void add_cmd(Device_t &device, Proto_cmd &cmd);

    public:
        template <typename Device_t>
        void init(Device_t &device);
    };

    template <typename Device_t>
    inline void Proto_init::add_cmd(Device_t &device, Proto_cmd &cmd)
    {
        const auto &name = cmd.get_name();
        device.cmds.add_cmd(name, std::move(cmd));
    }

    template <typename Device_t>
    inline void Proto_init::init(Device_t &device)
    {
        /* Modules */
        auto greeter = std::make_unique<Greeter<Device_t>>(device);
        auto exchanger = std::make_unique<Exchanger<Device_t>>(device);

        /* Add commands */
        add_cmd(device, Keep_alive<Device_t>{device});
        add_cmd(device, Master_hi<Device_t>{device});
        add_cmd(device, Slave_hi<Device_t>{device});
        add_cmd(device, Set_baud_rate<Device_t>{device});
        add_cmd(device, Set_char_size<Device_t>{device});
        add_cmd(device, Set_parity<Device_t>{device});
        add_cmd(device, Set_stop_bits<Device_t>{device});
        add_cmd(device, Get_baud_rate<Device_t>{device});
        add_cmd(device, Get_char_size<Device_t>{device});
        add_cmd(device, Get_parity<Device_t>{device});
        add_cmd(device, Get_stop_bits<Device_t>{device});
        add_cmd(device, Packet_ack<Device_t>{device});
        add_cmd(device, No_number<Device_t>{device, *exchanger});
        add_cmd(device, Invalid_number<Device_t>{device, *exchanger});
        add_cmd(device, Disconnect_detect<Device_t>{device});

        /* Set modules */
        device.set_greeter(std::move(greeter));
        device.set_exchanger(std::move(exchanger));
    }
}