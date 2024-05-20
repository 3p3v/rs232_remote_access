#include <Ws_ext.hpp>

namespace Job_ctrl
{
    Ws_ext::Ws_ext(Worker_storage &ws)
        : ws{ws}
    {
    }

    Ws_ext::~Ws_ext() = default;
}