#pragma once

#include <Storage.hpp>
#include <Forwarder.hpp>

namespace Job_ctrl
{
    template <typename Worker_ptr_t>
    class Manager : virtual public Storage<Worker_ptr_t>, public Forwarder<Worker_ptr_t>
    {
    };
    
}