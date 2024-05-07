#pragma once

#include <Worker_ref.hpp>
#include <Storage.hpp>

using namespace Job_ctrl;

namespace Logic
{
    using Ext_storage_base = Storage<Worker_ref>;
    
    class Ext_storage : public Ext_storage_base
    {
        using Ext_storage_base::Ext_storage_base;
    };
}