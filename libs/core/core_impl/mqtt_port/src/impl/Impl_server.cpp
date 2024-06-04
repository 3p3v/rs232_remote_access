#include <Impl_server.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        void Impl_server::set_options(Server::Get_cont &&server, mqtt::connect_options &options)
        {
            if (server.size())
            {
                mqtt::ssl_options ssl_opt;

                server.for_each([&ssl_opt](auto &opt_)
                                {
                            auto opt = opt_.first;
                            auto arg = opt_.second;

                            switch (opt)
                            {
                            case Server::Option::cert:
                            {
                                ssl_opt.set_key_store(arg);
                                break;
                            }
                            case Server::Option::pem:
                            {
                                ssl_opt.set_private_key(arg);
                                break;
                            }
                            default:
                            {
                                throw std::logic_error("Tried to set unknown function.");
                                break;
                            }
                            }; });

                options.set_ssl(std::move(ssl_opt));
            }
        }
    }
}
