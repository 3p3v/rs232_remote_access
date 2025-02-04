#pragma once

#include <Device_prereq.hpp>

Logic::Device_prereq::Device_prereq(
    Notyfier &&notyfier,
    Remote_dev &&rec)
    : notyfier{std::move(notyfier)},
        dev(std::forward<Device_ptr_t>(dev)),
        rec_(std::move(rec))
{
}