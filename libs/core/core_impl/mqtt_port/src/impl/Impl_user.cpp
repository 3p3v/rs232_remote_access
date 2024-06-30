#include <Impl_user.hpp>
#include <algorithm>
#include <stdexcept>

namespace Mqtt_port
{
    namespace Impl
    {
        void Impl_user::set_options(User::Get_cont &&user, mqtt::connect_options &options)
        {
            // options.set_clean_session(true); // FIXME thwors error while connecting

            user.for_each([&options](auto &opt_)
                          {
                            auto opt = opt_.first;
                            auto arg = opt_.second;

                            switch (opt)
                            {
                            case User::Option::no_clean:
                            {
                                // options.set_clean_session(false); // TODO add option to read if true or false from string
                                break;
                            }
                            case User::Option::username:
                            {
                                options.set_user_name(arg);
                                break;
                            }
                            case User::Option::password:
                            {
                                options.set_password(arg);
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