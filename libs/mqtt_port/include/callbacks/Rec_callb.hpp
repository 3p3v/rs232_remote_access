#pragma once

#include <I_callb.hpp>

namespace Mqtt_port
{
    template <typename Cont_t, typename Rcallb>
    class Rec_callb final : public I_callb<Cont_t>
    {
        /* User callback for successful sending message */
        Rcallb rec_msg;
        std::shared_ptr<Validator> validator;

    public:
        Rec_callb(Impl::Controller &controller, std::shared_ptr<Validator> validator, Rcallb &&rec_msg);

        void fail() override;

        void success(const std::string &channel_name, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end) override;

    private:
        template <typename A_cont>
        void read(const std::string &channel_name, const typename A_cont::const_iterator begin, const typename A_cont::const_iterator end);
    };

    template <typename Cont_t, typename Rcallb>
    Rec_callb<Cont_t, Rcallb>::Rec_callb(Impl::Controller &controller, std::shared_ptr<Validator> validator, Rcallb &&rec_msg)
        : I_callb{controller}, validator{std::move(validator)}, rec_msg{std::move(rec_msg)}
    {
    }

    template <typename Cont_t, typename Rcallb>
    void Rec_callb<Cont_t, Rcallb>::fail()
    {
        throw std::runtime_error("Could not receive message!");
        controller.disconnect(time_to_disconnect);
    }

    template <typename Cont_t, typename Rcallb>
    void Rec_callb<Cont_t, Rcallb>::success(const std::string &channel_name, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end)
    {
        read<Cont_t>(channel_name, begin, end);
        rec_msg(channel_name, end - begin);
    }

    template <typename Cont_t, typename Rcallb>
    template <typename A_cont>
    void Rec_callb<Cont_t, Rcallb>::read(const std::string &channel_name, const typename A_cont::const_iterator begin, const typename A_cont::const_iterator end)
    {
        if (validator->validate(channel_name))
        {
            validator->get_exec(channel_name)->exec(channel_name, begin, end);
        }
        else
        {
            throw std::logic_error("Somehow tried to write to channel that client was not connected to!");
            controller.disconnect(time_to_disconnect);
        }
    }
}