#include <Interpreter.hpp>
/* Policies */
#include <Bigger_or_equal.hpp>
#include <Lower_or_equal.hpp>
#include <No_arg.hpp>
#include <Numbers_only.hpp>
#include <Alpha_only.hpp>

using namespace Cmd_ctrl;

namespace Logic
{
   Interpreter::Interpreter()
   {
      /* HI */
      add_cmd(std::string{master_hi_s},
              Command::Policies<No_arg>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.say_hi(); }));

      add_cmd(std::string{slave_hi_s},
              Command::Policies<No_arg>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { 
                                                                /* Delete timer */
                                                                helper.say_hi_compl();

                                                                /* Start keep alive timer */
                                                                helper.keep_alive_start();

                                                                /* Get settings if unknown */
                                                                if (!helper.get_helper().settings_known)
                                                                {
                                                                        helper.get_settings();
                                                                }
                                                                /* Else send settings */
                                                                else
                                                                {
                                                                        helper.set_baud_rate(helper.get_info().baud_rate);
                                                                        helper.set_parity(helper.get_info().parity);
                                                                        helper.set_char_size(helper.get_info().char_size);
                                                                        helper.set_stop_bits(helper.get_info().stop_bits);
                                                                } }));

      add_cmd(std::string{slave_keep_alive_s},
              Command::Policies<No_arg>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.keep_alive(); }));

      add_cmd(std::string{packet_ack_s},
              Command::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.ack_packet(*arg.data()); }));
      add_cmd(std::string{invalid_number_s},
              Command::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.resend(*arg.data()); }));
   }
}