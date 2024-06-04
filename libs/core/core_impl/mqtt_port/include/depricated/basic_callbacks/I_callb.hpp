#pragma once

#include <Callb.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Controller;
    }

    template <typename Cont_t>
    class I_callb : public Callb
    {
    public:
        I_callb(Impl::Controller &controller);
        void fail() = 0;
        virtual void success(const std::string &channel_name, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end) = 0;
    };

    template <typename Cont_t>
    I_callb<Cont_t>::I_callb(Impl::Controller &controller)
        : Callb{controller}
    {
    }
}