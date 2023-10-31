#include <Impl_user.hpp>
#include <algorithm>
#include <stdexcept>

namespace Mqtt_port
{
    namespace Impl
    {
        void Impl_user::set_options(Mqtt_port::User::Get_cont &user, mqtt::connect_options &options)
        {
            options.set_clean_session(true);
            
            user.for_each([](std::string &opt))
            
            std::for_each(user.get_options().begin(),
                          user.get_options().end(),
                          [&user, &options](auto opt)
                          {
                            switch (opt)
                            {
                            case User_opt::Option::no_clean:
                            {
                                options.set_clean_session(false);
                                break;
                            }
                            case User_opt::Option::username:
                            {
                                options.set_user_name(user.get_username());
                                break;
                            }
                            case User_opt::Option::password:
                            {
                                options.set_password(user.get_password());
                                break;
                            }
                            default:
                            {
                                throw std::logic_error("Tried to set unknown function.");
                                break;
                            }
                            };
                          });
        }
    }
}