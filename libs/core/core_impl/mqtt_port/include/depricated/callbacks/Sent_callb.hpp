#pragma once

#include <O_callb.hpp>

namespace Mqtt_port
{
    template <typename Scallb>
    class Sent_callb final : public O_callb
    {
        /* User callback for successful sending message */
        Scallb sent_msg;

    public:
        Sent_callb(Impl::Controller &controller, Scallb &&sent_msg);

        void fail() override;

        void success(const std::string &channel_name, size_t len) override;
    };

    template <typename Scallb>
    Sent_callb<Scallb>::Sent_callb(Impl::Controller &controller, Scallb &&sent_msg)
        : O_callb{controller}, sent_msg{std::move(sent_msg)}
    {
    }

    template <typename Scallb>
    void Sent_callb<Scallb>::fail()
    {
        throw std::runtime_error("Could not send message!");
        controller.disconnect(time_to_disconnect);
    }

    template <typename Scallb>
    void Sent_callb<Scallb>::success(const std::string &channel_name, size_t len)
    {
        sent_msg(channel_name, len);
    }
}