#pragma once

/* Device and extension management */
#include <Process_full.hpp>
#include <User_controller.hpp>
#include <Remote_dev.hpp>
/* Dummy for assurence that object exists as long as operations need it */
#include <Device_base.hpp>

namespace Logic
{
    /// @brief Dummy for storing in container
    class Device_prereq
    {
    public:
        using Device_weak_ptr = std::weak_ptr<Device_base>;

    protected:
        /// @brief User notyfication
        Notyfier notyfier;

        /// @brief Device record
        Remote_dev rec_;

        /// @brief Pointer to device holder (Device_base / Device_initializer)
        Device_weak_ptr dev;

        /// @brief Extension management
        Process_full manager{};

    public:
        /// @brief Control over device connection parameters ect.
        User_controller cont{manager.ext_forwarder};

        /// @brief Current settings and status
        const Remote_dev &rec{rec_};

        /// @brief 
        /// @tparam Device_ptr_t Device_weak_ptr
        /// @param notyfier User notyfication
        /// @param dev Pointer to device holder (Device_base / Device_initializer)
        /// @param rec Device record object
        template <typename Device_ptr_t>
        Device_prereq(
            Notyfier &&notyfier,
            Device_ptr_t &&dev,
            Remote_dev &&rec);

        Device_prereq(Device_prereq &&) = default;
        Device_prereq &operator=(Device_prereq &&) = default;
        Device_prereq(const Device_prereq &) = default;
        Device_prereq &operator=(const Device_prereq &) = default;
        virtual ~Device_prereq() = default;
    };

    template <typename Device_ptr_t>
    Device_prereq::Device_prereq(
        Notyfier &&notyfier,
        Device_ptr_t &&dev,
        Remote_dev &&rec)
        : notyfier{std::move(notyfier)},
          dev(std::forward<Device_ptr_t>(dev)),
          rec_(std::move(rec))
    {
    }
}