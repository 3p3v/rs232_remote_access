#include <Dev_cont.hpp>



Logic::Dev_cont::Dev_cont(Dev_cont &&dc)
    : devs{std::move(dc.devs)}, num{dc.num}
{
}

Logic::Dev_cont::~Dev_cont() = default;

// Logic::Dev_cont& Logic::Dev_cont::operator=(Dev_cont &&dc)
// {
//     *this = Dev_cont{std::move(dc)};
//     return *this;
// }


