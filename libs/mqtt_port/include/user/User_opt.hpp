#pragma once

#include <vector>

namespace Mqtt_port
{
    class User_opt
    {
    public:
        enum class Option
        {
            no_clean,
            username, 
            password
        };

    public:
        User_opt() = default;
        User_opt(std::initializer_list<Option> options);
        User_opt(User_opt&) = default;
        User_opt& operator=(User_opt&) = default;
        User_opt(User_opt&&) = default;
        User_opt& operator=(User_opt&&) = default;
        virtual ~User_opt() = 0;
    };
}