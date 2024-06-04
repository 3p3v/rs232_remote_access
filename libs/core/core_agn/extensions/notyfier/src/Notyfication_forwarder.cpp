#include <Notyfication_forwarder.hpp>
#include <Error_job.hpp>
#include <Debug_job.hpp>

namespace Logic
{   
    void Notyfication_forwarder::error(Dev_num dev, const std::exception &exc)
    {
        forward_job(Error_job{dev, exc});
    }

    void Notyfication_forwarder::debug(Dev_num dev, const std::string &info)
    {
        forward_job(Debug_job{dev, info});
    }
}
