#pragma once

#include <Unauthed_ext.hpp>
#include <Timer_cont.hpp>

using namespace Job_ctrl;

namespace Job_ctrl
{
    class Manager;
}

namespace Logic
{
    /// @brief Base functionality of any module implementing functionality of MQTT<->Serial communication
    /// @tparam Timer_t Timer implementation
    template <typename Timer_t>
    class Common_ext : public Unauthed_ext
    {
    protected:
        using Timers = Timer_cont<Timer_t>;

        /// @brief For setting action timeouts
        Timers timers;

        /// @brief 
        void add_restart_job() override;

        /// @brief
        void clear_timers();

    public:
        template <typename Forwarder_ptr_t>
        Common_ext(Forwarder_ptr_t&& manager);
        Common_ext(const Common_ext&) = delete;
        Common_ext& operator=(const Common_ext&) = delete;
        Common_ext(const Common_ext&) = default;
        Common_ext& operator=(const Common_ext&) = default;
        ~Common_ext() = 0;
    };

    template <typename Timer_t>
    template <typename Forwarder_ptr_t>
    inline Common_ext<Timer_t>::Common_ext(Forwarder_ptr_t &&manager)
        : Unauthed_ext{std::forward<Forwarder_ptr_t>(manager)}
    {
    }

    template <typename Timer_t>
    inline void Common_ext<Timer_t>::clear_timers()
    {
        timers.clear();
    }
}