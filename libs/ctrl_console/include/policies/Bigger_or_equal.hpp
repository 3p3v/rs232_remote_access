#pragma once

#include <Numbers_only.hpp>
#include <Bigger_than.hpp>
#include <Equal_to.hpp>
#include <Or.hpp>
#include <And.hpp>

namespace Cmd_ctrl
{
    template <int number>
    using Bigger_or_equal = And<Numbers_only, Or<Bigger_than<number>, Equal_to<number>>>;
}