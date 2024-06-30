#include <Collective_general.hpp>
#include <Notyfication_storage.hpp>

using namespace Logic;

namespace Impl
{
  Collective_general::Collective_general(Server::Cont server, User::Cont user, bool close_on_timeout, bool close_on_data_loss, bool close_on_protocol_error, bool debug)
      : debug_monitor{Info_getter{devs}, debug},
        exception_handler{
            Notyfication_forwarder{notifyer},
            Impl_remover{devs},
            Dev_getter{devs},
            close_on_timeout,
            close_on_data_loss,
            close_on_protocol_error},
        controller{std::move(server), std::move(user)}
  {
    /* Init notification mechanism */
    Notyfication_storage ns{notifyer};
    ns.add(debug_monitor);
    ns.add(exception_handler);

    controller.connect(
        []() {

        },
        [](int code) {
          throw std::runtime_error{"Disconnected from server, code: " + std::to_string(code) + "!"};
        },
        [](int code) {
          throw std::runtime_error{"Could not connect to server, code: " + std::to_string(code) + "!"};
        });

    while(!controller.is_connected()); // TODO wait for connection asynchroniously
  }
}
