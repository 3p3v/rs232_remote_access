#pragma once

#include <stdexcept>
#include <Notyfier_defs.hpp>
#include <Worker.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Interface for monitor notyfier
    class Notyfication_observer : public Worker
    {
    public:
        using Dev_num = Notyfier_defs::Dev_num;

        using Worker::Worker;
    };
}