#pragma once

#include <Dev_info.hpp>
#include <Impl_cont.hpp>
#include <Notyfier_defs.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Interface for adding new devices
    class Impl_adder_ref_base
    {
    public:
        using Dev_num = ::Logic::Notyfier_defs::Dev_num;

    public:
        /// @brief Add new device
        /// @param sett Devices local port settings
        /// @param write_access If user can send data to remote
        /// @param info Devices record
        virtual void add(Port_settings_storage &&sett, Remote_conf_port write_access, Dev_info &&info, Impl_cont &devs, Worker_storage& notyfier) = 0;

        Impl_adder_ref_base() = default;
        Impl_adder_ref_base(Impl_adder_ref_base&&) = default;
        Impl_adder_ref_base& operator=(Impl_adder_ref_base&&) = default;
        Impl_adder_ref_base(const Impl_adder_ref_base &) = default;
        Impl_adder_ref_base& operator=(const Impl_adder_ref_base &) = default;
        virtual ~Impl_adder_ref_base() = default;
    };
} // namespace Impl
