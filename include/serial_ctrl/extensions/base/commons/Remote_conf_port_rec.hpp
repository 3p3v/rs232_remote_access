#pragma once

namespace Logic
{
    enum class Remote_conf_port : bool
    {
        Configurable = true,
        Non_configurable = false
    };
    
    class Remote_conf_port_rec
    {
    public:
        /// @brief Configurable device?
        const Remote_conf_port conf_port{true};

        Remote_conf_port_rec(Remote_conf_port conf_port);
        Remote_conf_port_rec(Remote_conf_port_rec &&) = default;
        Remote_conf_port_rec &operator=(Remote_conf_port_rec &&) = default;
        Remote_conf_port_rec(const Remote_conf_port_rec &) = default;
        Remote_conf_port_rec &operator=(const Remote_conf_port_rec &) = default;
        virtual ~Remote_conf_port_rec() = 0;
    };
}