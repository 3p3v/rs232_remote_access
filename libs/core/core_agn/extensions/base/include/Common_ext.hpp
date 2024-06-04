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
        /// @brief 
        void add_restart_job() override;

        using Timers = Timer_cont;

        /// @brief For setting action timeouts
        Timers timers;

        /// @brief
        void clear_timers();

    public:
        template <typename Device_weak_ptr_t>
        Common_ext(Forwarder &&forwarder, Notyfier &&notyfier, Device_weak_ptr_t &&device_ptr);
        Common_ext(const Common_ext&) = delete;
        Common_ext& operator=(const Common_ext&) = delete;
        Common_ext(Common_ext&&) = default;
        Common_ext& operator=(Common_ext&&) = default;
        ~Common_ext() = 0;
    };

    template <typename Timer_t>
    template <typename Device_weak_ptr_t>
    inline Common_ext<Timer_t>::Common_ext(Forwarder &&forwarder, Notyfier &&notyfier, Device_weak_ptr_t &&device_ptr)
        : Unauthed_ext{std::move(forwarder), std::move(notyfier), std::forward<Device_weak_ptr_t>(device_ptr)}
    {
        add_restart_job();
    }

    template <typename Timer_t>
    void Common_ext<Timer_t>::add_restart_job()
    {
        // Add job for resetting all timers (used when there was error in communication with device)
        add_handler(
            Job_type::Urgent,
            Job_policies<>::make_job_handler<Restart_job>(
                [this](auto &job)
                {
                    timers.clear();
                }));
    }

    template <typename Timer_t>
    inline void Common_ext<Timer_t>::clear_timers()
    {
        timers.clear();
    }

    template <typename Timer_t>
    inline Common_ext<Timer_t>::~Common_ext() = default;
}