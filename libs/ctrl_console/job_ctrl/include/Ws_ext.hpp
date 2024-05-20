#pragma once

#include <Worker_storage.hpp>

namespace Job_ctrl
{
    class Ws_ext
    {
    protected:
        Worker_storage &ws;

    public:
        Ws_ext(Worker_storage &ws);
        Ws_ext(Ws_ext&&) = default;
        Ws_ext& operator=(Ws_ext&&) = default;
        Ws_ext(const Ws_ext&) = default;
        Ws_ext& operator=(const Ws_ext&) = default;
        virtual ~Ws_ext() = 0;
    };
}