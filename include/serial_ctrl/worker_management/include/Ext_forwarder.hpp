#pragma once

#include <Worker_ref.hpp>
#include <Forwarder.hpp>

using namespace Job_ctrl;

namespace Logic
{
    using Ext_forwarder_base = Forwarder<Worker_ref>;
    
    class Ext_forwarder : public Ext_forwarder_base
    {
        using Ext_forwarder_base::Ext_forwarder_base;
    };
}