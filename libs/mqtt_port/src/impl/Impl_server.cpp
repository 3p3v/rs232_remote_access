#include <Impl_server.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        void Impl_server::set_options(Server::Get_cont &&server, mqtt::connect_options &options)
        {
            mqtt::ssl_options ssl_opt;
            
            server.for_each([&ssl_opt](Server::Get_cont::Opt_tuple &opt)
                          {
                            switch (std::get<0>(opt))
                            {
                            case Server::Option::cert:
                            {
                                ssl_opt.set_key_store(std::get<1>(opt));
                                break;
                            }
                            case Server::Option::pem:
                            {
                                ssl_opt.set_private_key(std::get<1>(opt));
                                break;
                            }
                            default:
                            {
                                throw std::logic_error("Tried to set unknown function.");
                                break;
                            }
                            };
                          });
            // TODO enable later
            // options.set_ssl(std::move(ssl_opt));
        }
    }   
}
