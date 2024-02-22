#pragma once

#include <Forwarder.hpp>
#include <Remote_ext.hpp>

using namespace Job_ctrl;

namespace Job_ctrl
{
    class Manager;
}

namespace Logic
{
    /// @brief Base functionality of any module implementing functionality of MQTT<->Serial communication
    class Unauthed_ext : public Remote_ext
    {
        using Forwarder_ptr = std::weak_ptr<Manager>;

    private:
        /// @brief Used to send messages to other modules
        Forwarder_ptr manager;

    protected:
        void forward_job(Job &&job) override final;
        void forward_job(const Job &job) override final;

    public:
        template <typename Forwarder_ptr_t>
        Unauthed_ext(Forwarder_ptr_t&& manager);
        Unauthed_ext(const Unauthed_ext&) = delete;
        Unauthed_ext& operator=(const Unauthed_ext&) = delete;
        Unauthed_ext(const Unauthed_ext&) = default;
        Unauthed_ext& operator=(const Unauthed_ext&) = default;
        ~Unauthed_ext() = 0;
    };

    template <typename Forwarder_ptr_t>
    inline Remote_ext::Unauthed_ext(Forwarder_ptr_t &&manager)
        : Remote_ext{std::forward<Forwarder_ptr_t>(manager)}
    {
    }
}