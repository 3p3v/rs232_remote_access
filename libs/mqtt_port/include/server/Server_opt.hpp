#pragma once

#include <set>
#include <User_get_intf.hpp>

namespace Mqtt_port
{
    class Server_opt : public User_get_intf
    {
    public:
        enum class Option
        {
            ssl
        };

        using Options_cont_type = std::set<Option>;

    private:
        Options_cont_type options{};

    protected:
        void add_option(Option option);

    public:
        Server_opt() = default;
        Server_opt(std::initializer_list<Option> options);
        Options_cont_type& get_options();
    };
}