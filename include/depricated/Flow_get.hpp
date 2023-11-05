#pragma once

namespace Ip_serial
{
    class Flow_get
    {
    protected:
        size_t in{0};
        size_t out{0};

    public:
        size_t get_out() const;
        size_t get_in() const;

        Flow_get() = default;
        Flow_get(Flow_get &&) = default;
        Flow_get &operator=(Flow_get &&) = delete;
        Flow_get(Flow_get &) = default;
        Flow_get &operator=(Flow_get &) = delete;
        virtual ~Flow_get() = 0;
    };
}