#pragma once

#include <Exchanger_except.hpp>

namespace Logic
{
    class Exchanger_fatal_except : public Exchanger_except
    {
        using Exchanger_except::Exchanger_except;
    };
 
}