#pragma once

#include <Monitor.hpp>

class Monitor_impl : public Monitor
{
private:
    /* data */
public:
    void wake(const Device_ptr &device) override
    {

    }

    void wake_delete(const Device_ptr &device) override
    {

    }

    void validate(const std::string &name) override
    {

    }

    void error(Errors type) override {}
    void error(Errors type, std::string&& what) override {}
    void error(Errors type, const std::string& what) override {}
    void error(Errors type, int code) override {}
    void error(Errors type, int code, std::string&& what) override {}
    void error(Errors type, int code, const std::string& what) override {}
};

