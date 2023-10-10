#pragma once

#include <Serial.hpp>
#include <memory>
#include <User_channel_monitor.hpp>

using namespace Serial_port;

class Local_serial final : public Serial
{
    std::shared_ptr<User_channel_monitor_adder_out> monitor;

    /* Callbacks */
    virtual void write_callback(std::size_t write_len) override;
    virtual void read_callback(const Data &data, std::size_t write_len) override;
    virtual void error_callback(const unsigned int code, const std::string &err) override;

public:
    Local_serial(Io_context_ptr io_context_);
    ~Local_serial() = default;
};
