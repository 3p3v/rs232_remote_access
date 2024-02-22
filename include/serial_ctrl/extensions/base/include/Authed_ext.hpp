#pragma once

#include <Process_manager.hpp>
#include <Remote_ext.hpp>

using namespace Job_ctrl;

namespace Logic
{
    /// @brief Extension with ability to add other modules
    class Authed_ext : public Remote_ext
    {
    public:
        using Manager_ptr = std::weak_ptr<Process_manager>;

    protected:
        /// @brief Used to send messages to other modules
        Manager_ptr manager;

        void forward_job(Job &&job) override final;
        void forward_job(const Job &job) override final;

    public:
        template <typename Manager_ptr_t>
        Authed_ext(Manager_ptr_t&& manager);
        Authed_ext(const Authed_ext&) = delete;
        Authed_ext& operator=(const Authed_ext&) = delete;
        Authed_ext(const Authed_ext &) = default;
        Authed_ext& operator=(const Authed_ext&) = default;
        ~Authed_ext() = 0;
    };

    template <typename Manager_ptr_t>
    inline Authed_ext::Authed_ext(Manager_ptr_t &&manager)
        : Remote_ext{std::forward<Manager_ptr_t>(manager)}
    {
    }
}