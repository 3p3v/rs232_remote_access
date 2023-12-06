#pragma once

#include <array>
#include <atomic>
#include <Serial.hpp>

namespace Ip_serial
{
    class Mqtt_msg
    {
    public:
        static constexpr size_t max_size{1300}; 

    public:
        unsigned char id;
        Serial_port::Serial::Data data;
        size_t data_len;
        std::atomic<bool> freed{true};
        /* If data is being changed now */
        std::atomic<bool> used{false};
    };

}