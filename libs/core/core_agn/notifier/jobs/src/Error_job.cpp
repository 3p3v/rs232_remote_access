#include <Error_job.hpp>

const std::exception &Logic::Error_job::get_exc() const noexcept
{
    return *exc;
}

Logic::Error_job::Error_job(const Error_job &ej)
    : Error_job{ej.num, ej.get_exc()}
{
}

Logic::Error_job &Logic::Error_job::operator=(const Error_job &ej)
{
    num = ej.num;
    exc.reset(ej.exc.get());

    return *this;
}
