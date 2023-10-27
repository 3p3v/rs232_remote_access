#pragma once

#include <unordered_map>
#include <string>

class Flow_monitor_adder_in
{
public:
    virtual void add_in(const std::string& dev_name, size_t rec_len) = 0;
};

class Flow_monitor_adder_out
{
public:
    virtual void add_out(const std::string& dev_name, size_t write_len) = 0;
};

class Flow_monitor_getter
{
public:
    virtual size_t get_out(const std::string& dev_name) = 0;
    virtual size_t get_in(const std::string& dev_name) = 0;
};

class Flow_monitor final : public Flow_monitor_adder_in,
                                   public Flow_monitor_adder_out,
                                   public Flow_monitor_getter
{
    std::unordered_map<std::string, std::pair<size_t, size_t>> serials;

public:
    size_t get_out(const std::string& dev_name);
    size_t get_in(const std::string& dev_name);
    void add_in(const std::string& dev_name, size_t rec_len);
    void add_out(const std::string& dev_name, size_t write_len);
};