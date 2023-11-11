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

    void error(const Exception::Exception &except) override
    {

    }
};

