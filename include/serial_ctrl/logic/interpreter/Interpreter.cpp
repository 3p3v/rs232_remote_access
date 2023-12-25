#include <Interpreter.hpp>
/* Policies */
#include <Bigger_or_equal.hpp>
#include <Lower_or_equal.hpp>

namespace Logic
{
   Interpreter::Interpreter()
   {
      /* HI */
      add_cmd(std::string{master_hi_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.say_hi(); }));

      add_cmd(std::string{slave_hi_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
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
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.keep_alive(); }));

      /* Add commands used to control UART settings */
      add_cmd(std::string{set_baud_rate_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.set_baud_rate(); }));
      add_cmd(std::string{get_baud_rate_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.set_baud_rate_compl(arg); }));

      add_cmd(std::string{set_parity_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.set_parity(); }));
      add_cmd(std::string{get_parity_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.set_parity_compl(arg); }));

      add_cmd(std::string{set_char_size_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.set_char_size(); }));
      add_cmd(std::string{get_char_size_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.set_char_size_compl(arg); }));

      add_cmd(std::string{set_stop_bits_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.set_stop_bits(); }));
      add_cmd(std::string{get_stop_bits_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.set_stop_bits_compl(arg); }));

      add_cmd(std::string{packet_ack_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.ack_packet(*arg.data()); }));
      add_cmd(std::string{invalid_number_s},
              Handle_t::Policies<Numbers_only>::Dyn_handle([](Ip_serial_ctrl &helper,
                                                              const std::string &arg)
                                                           { helper.resend(*arg.data()); }));
   }
}