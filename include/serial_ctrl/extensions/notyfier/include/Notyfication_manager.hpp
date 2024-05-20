#pragma once

#include <Notyfication_forwarder.hpp>
#include <Notyfication_storage.hpp>

using namespace Job_ctrl;

namespace Logic
{
    class Notyfication_dummy
    {
    public:
        Worker_storage ws;

        Notyfication_dummy() = default;
        Notyfication_dummy(Notyfication_dummy&&) = default;
        Notyfication_dummy& operator=(Notyfication_dummy&&) = default;
        Notyfication_dummy(const Notyfication_dummy&) = default;
        Notyfication_dummy& operator=(const Notyfication_dummy&) = default;
        virtual ~Notyfication_dummy() = 0;
    };
    
    class Notyfication_manager 
        : Notyfication_dummy, public Notyfication_storage, public Notyfication_forwarder
    {
    public:
        Notyfication_manager();
    };
} // namespace Logic
