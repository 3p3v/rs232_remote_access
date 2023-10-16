#pragma once

#include <Defs.hpp>
#include <Def.hpp>
// #include <impl\Controller.hpp>
#include <Connector.hpp>

namespace Mqtt_port
{
    namespace Impl
    {
        class Controller;
    }

    class Callb : protected Defs
    {
    protected:
        Impl::Controller &controller;

    public:
        Callb(Impl::Controller &controller);
        Callb(Callb &&) = default;
        Callb &operator=(Callb &&) = default;
        Callb(Callb &) = default;
        Callb &operator=(Callb &) = default;
        ~Callb() = default;
        virtual inline void fail() = 0;
    };

    class I_callb : public Callb
    {
    public:
        I_callb(Impl::Controller &controller);
        inline void fail() = 0;
        virtual inline void success(const std::string &channel_name, const Data &data) = 0;
        // virtual inline void success() = delete;
    };

    class O_callb : public Callb
    {
    public:
        O_callb(Impl::Controller &controller);
        inline void fail() = 0;
        virtual inline void success(const std::string &channel_name, size_t len) = 0;
        // virtual inline void success() = delete;
    };

    class C_callb : public Callb
    {
    protected:
        std::shared_ptr<Connector> connector;

    public:
        C_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector);
        inline void fail() = 0;
        virtual inline void success() = 0;
        // virtual inline void success() = delete;
    };
}