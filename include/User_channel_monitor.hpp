#pragma once

class User_channel_monitor_adder_in
{
public:
    virtual void add_in(size_t rec_len) = 0;
};

class User_channel_monitor_adder_out
{
public:
    virtual void add_out(size_t write_len) = 0;
};

class User_channel_monitor_getter
{
public:
    virtual size_t get_out() = 0;
    virtual size_t get_in() = 0;
};

class User_channel_monitor final : public User_channel_monitor_adder_in,
                                   public User_channel_monitor_adder_out,
                                   public User_channel_monitor_getter
{
    size_t data_in = 0;
    size_t data_out = 0;
public:
    size_t get_out();
    size_t get_in();
    void add_in(size_t rec_len);
    void add_out(size_t write_len);
};