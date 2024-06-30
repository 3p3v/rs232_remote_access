#pragma once

/* Device record */
#include <Remote_conf_port_rec.hpp>
#include <Remote_dev.hpp>
#include <Dev_info.hpp>

namespace Impl
{
    class Info_pack_base
    {
    public:
        /// @brief Access type
        Remote_conf_port write_access{Remote_conf_port::Configurable};

        /// @brief Serial port options
        Port_settings_storage sett;

        /// @brief Port and channel names
        Dev_info info;

        Info_pack_base(Dev_info &&info);
        Info_pack_base(Info_pack_base &&) = default;
        Info_pack_base& operator=(Info_pack_base &&) = default;
        Info_pack_base(const Info_pack_base &) = default;
        Info_pack_base& operator=(const Info_pack_base &) = default;
        virtual ~Info_pack_base() = default;
    };
} // namespace Impl
