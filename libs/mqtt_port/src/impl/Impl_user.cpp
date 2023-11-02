#include <Impl_user.hpp>
#include <algorithm>
#include <stdexcept>

namespace Mqtt_port
{
    namespace Impl
    {
        void Impl_user::set_options(User::Get_cont &&user, mqtt::connect_options &options)
        {
            options.set_clean_session(true);

            user.for_each([&options](User::Get_cont::Opt_tuple &opt)
                          {
                            switch (std::get<0>(opt))
                            {
                            case User::Option::no_clean:
                            {
                                options.set_clean_session(false);
                                break;
                            }
                            case User::Option::username:
                            {
                                options.set_user_name(std::get<1>(opt));
                                break;
                            }
                            case User::Option::password:
                            {
                                options.set_password(std::get<1>(opt));
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