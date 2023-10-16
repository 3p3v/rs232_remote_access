#include <impl/User_set_opt.hpp>
#include <algorithm>
#include <stdexcept>

namespace Mqtt_port
{
    namespace Impl
    {
        void User_set_opt::set_options(User_opt &user, mqtt::connect_options &options)
        {
            std::for_each(user.get_options().begin(),
                          user.get_options().end(),
                          [&user, &options](auto opt)
                          {
                            switch (opt)
                            {
                            case User_opt::Option::clean:
                            {
                                options.set_clean_session(true);
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