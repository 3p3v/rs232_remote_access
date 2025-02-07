#include <Collective_general.hpp>
#include <Notyfication_storage.hpp>

using namespace Logic;

namespace Impl
{
  Collective_general::Collective_general(std::unique_ptr<Impl_adder_ref_base> &&adder , bool close_on_timeout, bool close_on_data_loss, bool close_on_protocol_error, bool debug)
      : debug_monitor{Info_getter{devs}, debug},
        exception_handler{
            Notyfication_forwarder{notifier},
            Impl_remover{devs},
            Dev_getter{devs},
            close_on_timeout,
            close_on_data_loss,
            close_on_protocol_error},
        adder{std::move(adder)}
  {
    /* Init notification mechanism */
    Notyfication_storage ns{notifier};
    ns.add(debug_monitor);
    ns.add(exception_handler);
  }
}
