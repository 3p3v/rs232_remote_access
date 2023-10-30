#pragma once

#include <string>

namespace Mqtt_port
{   
    class Executor
    {
    public:
        using Data = std::string;

    public:
        Executor(Executor&&) = default;
        Executor& operator=(Executor&&) = default;
        Executor(Executor&) = delete;
        Executor& operator=(Executor&) = delete;
        virtual ~Executor() = default;

        virtual void exec(const std::string &ch_name, const Executor::Data::const_iterator begin, const Executor::Data::const_iterator end) = 0;
    };
}