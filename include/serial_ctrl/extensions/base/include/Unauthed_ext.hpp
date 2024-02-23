#pragma once

#include <Remote_ext.hpp>
#include <Process_manager.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Base functionality of any module implementing functionality of MQTT<->Serial communication
    class Unauthed_ext : public Remote_ext<Ext_forwarder>
    {
    public:
        template <typename Forwarder_ptr_t>
        Unauthed_ext(Forwarder_ptr_t&& manager);
        Unauthed_ext(const Unauthed_ext&) = delete;
        Unauthed_ext& operator=(const Unauthed_ext&) = delete;
        Unauthed_ext(Unauthed_ext&&) = default;
        Unauthed_ext& operator=(Unauthed_ext&&) = default;
        ~Unauthed_ext() = 0;
    };

    template <typename Forwarder_ptr_t>
    inline Unauthed_ext::Unauthed_ext(Forwarder_ptr_t &&manager)
        : Remote_ext{std::forward<Forwarder_ptr_t>(manager)}
    {
    }
}