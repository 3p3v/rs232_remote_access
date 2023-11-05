#pragma once

namespace Ip_serial
{
    class Flow_set
    {
    public:
        virtual void add_out(size_t write_len) = 0;
        virtual void add_in(size_t rec_len) = 0;

        Flow_set() = default;
        Flow_set(Flow_set &&) = default;
        Flow_set &operator=(Flow_set &&) = delete;
        Flow_set(Flow_set &) = default;
        Flow_set &operator=(Flow_set &) = delete;
        virtual ~Flow_set() = default;
    };
}