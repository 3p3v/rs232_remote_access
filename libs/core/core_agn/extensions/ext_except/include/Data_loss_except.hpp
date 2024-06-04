#pragma once

#include <Ext_except.hpp>

namespace Logic
{
    class Data_loss_except : public Ext_except
    {
        using Ext_except::Ext_except;
    };
 
}