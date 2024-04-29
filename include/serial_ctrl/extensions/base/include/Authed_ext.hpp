#pragma once

#include <Process_manager.hpp>
#include <Remote_ext.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Extension with ability to add other modules
    class Authed_ext : public Remote_ext<Process_manager>
    {
    public:
        /// @brief Reset all extensions
        void reset_exts_job();

        template <typename Manager_ptr_t>
        Authed_ext(Manager_ptr_t &&manager);
        Authed_ext(const Authed_ext &) = delete;
        Authed_ext &operator=(const Authed_ext &) = delete;
        Authed_ext(Authed_ext &&) = default;
        Authed_ext &operator=(Authed_ext &&) = default;
        ~Authed_ext() = 0;
    };

    inline void Authed_ext::Authed_ext::reset_exts_job()
    {
        forward_job(Restart_job{});
    }

    template <typename Manager_ptr_t>
    inline Authed_ext::Authed_ext(Manager_ptr_t &&manager)
        : Remote_ext{std::forward<Manager_ptr_t>(manager)}
    {
    }
}