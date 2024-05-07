#pragma once

#include <Port_settings_storage.hpp>
#include <Remote_conf_port_rec.hpp>

namespace Logic
{
    class Remote_record : virtual public Remote_conf_port_rec
    {
    public:
        static constexpr auto all_established{4};

        /// @brief Configurable device?
        bool settings_known{true};
        Port_settings_storage port_settings{};
        unsigned char params_established{};
        bool record_active{false};

        Remote_record() = default;
        explicit Remote_record(Remote_conf_port conf_port);
        explicit Remote_record(const Port_settings_storage &port_settings);
        explicit Remote_record(Port_settings_storage &&port_settings);
        Remote_record(const Port_settings_storage &port_settings, Remote_conf_port conf_port);
        Remote_record(Port_settings_storage &&port_settings, Remote_conf_port conf_port);

        template <
            typename... Params_t>
        Remote_record(Params_t &&...params)
            : port_settings{std::forward<Params_t>(params)...}
        {
        }

        template <
            typename... Params_t>
        Remote_record(Params_t &&...params, Remote_conf_port conf_port)
            : Remote_conf_port_rec{conf_port}, port_settings{std::forward<Params_t>(params)...}
        {
        }
    };

    // template <typename... Params_t>
    // inline Remote_record::Remote_record(Params_t &&...params)
    //     : port_settings{std::forward<Params_t>(params)...}
    // {
    // }

    // template <typename... Params_t>
    // inline Remote_record::Remote_record(Params_t &&...params, Remote_conf_port conf_port)
    //     : Remote_conf_port_rec{conf_port}, port_settings{std::forward<Params_t>(params)...}
    // {
    // }
}