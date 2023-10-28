#pragma once

#include <User_opt.hpp>
#include <User_get_intf.hpp>
#include <string>

namespace Mqtt_port
{
    class User final : public User_opt
    {
        std::string id;
        std::string username;
        std::string password;

    public:
        User() = default;
        
        template <typename S1, typename S2, typename S3>
        User(S1 &&username, S2 &&password, S3 &&id)
            : username{std::forward<S1>(username)},
              password{std::forward<S2>(password)},
              id{std::forward<S3>(id)},
              User_opt{User_opt::Option::username,
                      User_opt::Option::password}
        {
        }

        template<typename Str>
        void set_id(Str &&id)
        {
            this->id = std::forward<Str>(id);
            add_option(User_opt::Option::no_clean);
        }

        template<typename Str>
        void set_username(Str &&username)
        {
            this->username = std::forward<Str>(username);
            add_option(User_opt::Option::username);
        }

        template<typename Str>
        void set_password(Str &&password)
        {
            this->password = std::forward<Str>(password);
            add_option(User_opt::Option::password);
        }

        std::string get_username() override;
        std::string get_password() override;
        std::string get_id() override;
    };
}