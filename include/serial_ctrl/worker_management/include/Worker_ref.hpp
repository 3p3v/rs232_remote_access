#pragma once

#include <Worker.hpp>
#include <memory>

namespace Logic
{   
    using Worker_ref = std::shared_ptr<Job_ctrl::Worker>;
}