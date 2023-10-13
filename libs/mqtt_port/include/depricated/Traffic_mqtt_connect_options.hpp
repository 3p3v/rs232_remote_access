#pragma once

#include <set>

namespace Mqtt_port
{
    class Traffic_mqtt_connect_usr_options
    {
    public:
        enum class Option
        {
            clean,
            username, 
            password
        };

        using Options_cont_type = std::set<Option>;

    private:
        Options_cont_type options{};

    protected:
        void add_option(Option option);

    public:
        Options_cont_type& get_options();
    };
}