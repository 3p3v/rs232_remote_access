#include <Ip_console.hpp>
/* Policies */
#include <Numbers_only.hpp>

namespace Ip_serial
{
    Ip_console::Ip_console()
    {
        /* Add commands used to control UART settings */
        add_cmd(std::string{set_baud_rate_s},
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                        /*Set timer*/ }));
        add_cmd(Ip_defs::ok_trans(set_baud_rate_s),
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                            auto arg_ = std::strtoul(arg.data(), nullptr, 0);
                                                                            info.info.baud_rate = arg_;
                                                                            info.serial->set_baud_rate(arg_); }));

        add_cmd(std::string{set_parity_s},
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                        /*Set timer*/ }));
        add_cmd(Ip_defs::ok_trans(set_parity_s),
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                            auto arg_ = parity_trans(arg);
                                                                            info.info.parity = arg_;
                                                                            info.serial->set_parity(arg_); }));

        add_cmd(std::string{set_char_size_s},
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                        /*Set timer*/ }));
        add_cmd(Ip_defs::ok_trans(set_char_size_s),
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                            auto arg_ = std::strtoul(arg.data(), nullptr, 0);
                                                                            info.info.char_size = arg_;
                                                                            info.serial->set_char_size(arg_); }));

        add_cmd(std::string{set_flow_ctrl_s},
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                        /*Set timer*/ }));
        add_cmd(Ip_defs::ok_trans(set_flow_ctrl_s),
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                            auto arg_ = flow_ctrl_trans(arg);
                                                                            info.info.flow_ctrl = arg_;
                                                                            info.serial->set_flow_ctrl(arg_); }));

        add_cmd(std::string{set_stop_bits_s},
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                        /*Set timer*/ }));
        add_cmd(Ip_defs::ok_trans(set_stop_bits_s),
                Handle_t::Policies<Numbers_only>::Dyn_handle([](Serial_ctrl_helper &info,
                                                                const std::string &arg)
                                                             { 
                                                                            auto arg_ = stop_bits_trans(arg);
                                                                            info.info.stop_bits = arg_;
                                                                            info.serial->set_stop_bits(arg_); }));
    }
}