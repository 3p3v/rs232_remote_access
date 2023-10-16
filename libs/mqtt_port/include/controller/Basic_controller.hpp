#pragma once

#include <memory>
#include <stdexcept>
#include <Sender_intf.hpp>
#include <Disconnector_intf.hpp>
// #include <Receiver.hpp>
#include <Sender.hpp>
#include <Connector.hpp>
#include <Def.hpp>
#include <Defs.hpp>
#include <User_set_opt.hpp>
// #include <Traffic_channel_connector.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    // template <>
    // class Impl_controller
    // {
    // public:
    //     Impl_controller()
    //     void write(const std::string &channel_name, const Data &data, std::size_t write_len);
    // };

    template <typename Scallb, typename Rcallb>
    class Basic_controller;

    template <typename Scallb, typename Rcallb>
    Basic_controller(Scallb &&, Rcallb &&) -> Basic_controller<Scallb, Rcallb>;

    class Callb : protected Defs
    {
    public:
        Callb()
        {
        }

        virtual inline void fail() = 0;
    };

    class IO_callb : public Callb
    {
    public:
        inline void fail() = 0; 
        virtual inline void success(const std::string &channel_name, size_t len) = 0;
        // virtual inline void success() = delete;
    };

    class C_callb : public Callb
    {
    protected:
        std::shared_ptr<Connector> connector;
    public:
        C_callb(std::shared_ptr<Connector> connector)
            : connector(std::move(connector))
        {
        }

        inline void fail() = 0; 
        virtual inline void success() = 0;
        // virtual inline void success() = delete;
    };

    template <typename Scallb>
    class Sent_callb final : public IO_callb
    {
        /* User callback for successful sending message */
        Scallb sent_msg;

    public:
        Sent_callb(Scallb &&sent_msg)
            : sent_msg{std::move(sent_msg)}
        {
        }

        inline void fail() override
        {
            throw std::runtime_error("Could not send message!");
        }

        inline void success(const std::string &channel_name, size_t len) override
        {
            sent_msg(channel_name, len);
        }
    };

    template <typename Rcallb>
    class Rec_callb final : public IO_callb
    {
        /* User callback for successful sending message */
        Rcallb rec_msg;

    public:
        Rec_callb(Rcallb &&sent_msg)
            : rec_msg{std::move(sent_msg)}
        {
        }

        inline void fail() override
        {
            throw std::runtime_error("Could not send message!");
        }

        inline void success(const std::string &channel_name, size_t len) override
        {
            rec_msg(channel_name, len);
        }
    };

    class Channel_conn_callb final : public C_callb
    {
    public:
        Channel_conn_callb(std::shared_ptr<Connector> connector)
            : C_callb{std::move(connector)}
        {
        }

        inline void fail() override
        {
            throw std::runtime_error("Could not subscribe to channel!");
        }

        inline void success() override
        {
            
        }
    };

    class Conn_callb : public C_callb
    {
    public:
        Conn_callb(std::shared_ptr<Connector> connector)
            : C_callb{std::move(connector)}
        {
        }

        inline void fail() override
        {
            throw std::runtime_error("Could not connect to broker!");
        }

        inline void success() override
        {
            if (!connector->remaining_channels())
                throw std::logic_error("No channels to connect were specyfied!");
        }
    };

    template <typename Scallb, typename Rcallb>
    class Basic_controller : public Sender_intf, public Disconnector_intf
    {
    protected:
        /* Current state */
        // bool started = false;

        /* Logic behind channels */
        std::shared_ptr<Validator> validator;
        // std::shared_ptr<Receiver> receiver;
        std::shared_ptr<Connector> connector;
        /* User callbacks */
        Sent_callb<Scallb> sent_callb;
        Rec_callb<Rcallb> rec_callb;
        Conn_callb conn_callb;
        Channel_conn_callb channel_conn_callb;

        virtual void run_handle() = 0;
        virtual void write_handle(const std::string &channel_name, const Data &data, std::size_t write_len) = 0;
        void read(const std::string &channel_name, std::vector<unsigned char> data);

    public:
        Basic_controller(Scallb &&sent_msg, Rcallb &&rec_msg);
        Basic_controller(Basic_controller &&) = default;
        Basic_controller &operator=(Basic_controller &&) = default;
        Basic_controller(const Basic_controller &) = delete;
        Basic_controller &operator=(const Basic_controller &) = delete;
        virtual ~Basic_controller() = default;

        void run();
        void write(const std::string &channel_name, const Data &data, std::size_t write_len) override final;
        // virtual void disconnect(Time time) = 0;

        template <class E, typename S, typename... E_rgs>
        void add_channel(S &&channel_name, E_rgs &&...e_args);
    };

    template <typename Scallb, typename Rcallb>
    void Basic_controller<Scallb, Rcallb>::run()
    {
        // started = true;
        if (!connector->load_channels())
            throw std::logic_error("No channels to connect were specyfied!");

        run_handle();
    }

    template <typename Scallb, typename Rcallb>
    Basic_controller<Scallb, Rcallb>::Basic_controller(Scallb &&sent_msg, Rcallb &&rec_msg)
        : validator{new Validator{}},
          connector{new Connector{validator}},
          sent_callb{std::move(sent_msg)},
          rec_callb{std::move(rec_msg)},
          conn_callb{connector},
          channel_conn_callb{connector}
    {
    }

    template <typename Scallb, typename Rcallb>
    void Basic_controller<Scallb, Rcallb>::read(const std::string &channel_name, std::vector<unsigned char> data)
    {
        if (validator->validate(channel_name))
            validator->get_exec(channel_name).get()->exec(data);
        else
            throw std::logic_error("Somehow tried to write to channel that client was not connected to!");
    }

    template <typename Scallb, typename Rcallb>
    void Basic_controller<Scallb, Rcallb>::write(const std::string &channel_name, const Data &data, std::size_t write_len)
    {
        if (validator->validate(channel_name))
        {
            write_handle(channel_name, data, write_len);
        }
        else
        {
            throw std::runtime_error("Tried to write to unknown channel " + channel_name + ".");
        }
    }

    template <typename Scallb, typename Rcallb>
    template <class E, typename S, typename... E_rgs>
    void Basic_controller<Scallb, Rcallb>::add_channel(S &&channel_name, E_rgs &&...e_args)
    {
        validator->add_channel<E>(std::forward<S>(channel_name), std::forward<E_rgs>(e_args)...);
    }

}