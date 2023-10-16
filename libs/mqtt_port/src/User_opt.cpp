#include <User_opt.hpp>
#include <set>

namespace Mqtt_port
{
    User_opt::User_opt(std::initializer_list<Option> options)
        : options{options}
    {
    }

    void User_opt::add_option(Option option)
    {
        options.emplace(option);
    }

    User_opt::Options_cont_type &User_opt::get_options()
    {
        return options;
    }

}