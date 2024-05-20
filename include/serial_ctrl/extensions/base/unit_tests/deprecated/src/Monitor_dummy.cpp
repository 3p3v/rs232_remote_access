#include <Monitor_dummy.hpp>

namespace Logic
{
    void Monitor_dummy::error(Dev_num dev, const std::exception &exc)
    {
    }

    void Monitor_dummy::debug(Dev_num dev, const std::exception &exc)
    {
    }
}