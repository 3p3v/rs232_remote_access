#include <Error_job.hpp>

Logic::Error_job::Error_job(Dev_num num, const std::exception& exc)
    : num{num}, exc{exc}
{
}