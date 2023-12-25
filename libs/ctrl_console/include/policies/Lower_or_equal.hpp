#pragma once

#include <Numbers_only.hpp>
#include <Lower_than.hpp>
#include <Equal_to.hpp>
#include <Or.hpp>
#include <And.hpp>

namespace Cmd_ctrl
{
    template <int number>
    using Lower_or_equal = And<Numbers_only, Or<Lower_than<number>, Equal_to<number>>>;
}