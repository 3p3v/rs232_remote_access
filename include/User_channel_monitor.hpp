#pragma once

#include <unordered_map>
#include <string>

class User_channel_monitor_adder_in
{
public:
    virtual void add_in(const std::string& dev_name, size_t rec_len) = 0;
};

class User_channel_monitor_adder_out
{
public:
    virtual void add_out(const std::string& dev_name, size_t write_len) = 0;
};

class User_channel_monitor_getter
{
public:
    virtual size_t get_out(const std::string& dev_name) = 0;
    virtual size_t get_in(const std::string& dev_name) = 0;
};

class User_channel_monitor final : public User_channel_monitor_adder_in,
                                   public User_channel_monitor_adder_out,
                                   public User_channel_monitor_getter
{
    std::unordered_map<std::string, std::pair<int, int>> serials;

public:
    size_t get_out();
    size_t get_in();
    void add_in(size_t rec_len);
    void add_out(size_t write_len);
};