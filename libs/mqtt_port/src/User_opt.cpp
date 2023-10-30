#include <User_opt.hpp>
#include <set>

namespace Mqtt_port
{
    User_opt::User_opt(std::initializer_list<Option> options)
        : options{options}
    {
    }

    User_opt::~User_opt()
    {

    }

}