#pragma once

#include <memory>
#include <stdexcept>
#include <Sender_intf.hpp>
#include <Callbacks.hpp>
#include <Disconnector_intf.hpp>
// #include <Receiver.hpp>
#include <Controller.hpp>
#include <Sender.hpp>
#include <Connector.hpp>
#include <Def.hpp>
#include <Defs.hpp>
#include <User_set_opt.hpp>
// #include <Traffic_channel_connector.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    template <typename Scallb, typename Rcallb>
    class Basic_controller;

    template <typename Scallb, typename Rcallb>
    Basic_controller(Scallb &&, Rcallb &&) -> Basic_controller<Scallb, Rcallb>;

    template <typename Scallb>
    class Sent_callb final : public O_callb
    {
        /* User callback for successful sending message */
        Scallb sent_msg;

    public:
        Sent_callb(Impl::Controller &controller, Scallb &&sent_msg)
            : O_callb{controller}, sent_msg{std::move(sent_msg)}
        {
        }

        inline void fail() override
        {
            throw std::runtime_error("Could not send message!");
            controller.disconnect(time_to_disconnect);
        }

        inline void success(const std::string &channel_name, size_t len) override
        {
            sent_msg(channel_name, len);
        }
    };

    template <typename Rcallb>
    class Rec_callb final : public I_callb
    {
        /* User callback for successful sending message */
        Rcallb rec_msg;
        std::shared_ptr<Validator> validator;

        inline void read(const std::string &channel_name, Data data)
        {
            if (validator->validate(channel_name))
            {
                validator->get_exec(channel_name).get()->exec(data);
            }
            else
            {
                throw std::logic_error("Somehow tried to write to channel that client was not connected to!");
                controller.disconnect(time_to_disconnect);
            }
        }

    public:
        Rec_callb(Impl::Controller &controller, std::shared_ptr<Validator> validator, Rcallb &&sent_msg)
            : I_callb{controller}, validator{std::move(validator)}, rec_msg{std::move(sent_msg)}
        {
        }

        inline void fail() override
        {
            throw std::runtime_error("Could not send message!");
            controller.disconnect(time_to_disconnect);
        }

        inline void success(const std::string &channel_name, const Data &data) override
        {
            read(channel_name, data);
            rec_msg(channel_name, data.size());
        }
    };

    class Channel_conn_callb final : public C_callb
    {
    public:
        Channel_conn_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector)
            : C_callb{controller, std::move(connector)}
        {
        }

        inline void fail() override
        {
            throw std::runtime_error("Could not subscribe to channel!");
            controller.disconnect(time_to_disconnect);
        }

        inline void success() override
        {
            if (connector->remaining_channels())
                controller.subscribe(connector->get_channel_to_con(), qos);
        }
    };

    class Conn_callb : public C_callb
    {
        // std::shared_ptr<Channel_conn_callb> channel_conn_callb;

    public:
        Conn_callb(Impl::Controller &controller, std::shared_ptr<Connector> connector)
            : C_callb{controller, std::move(connector)}//, channel_conn_callb{new Channel_conn_callb{controller, connector}}
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
            else
                controller.subscribe(connector->get_channel_to_con(), qos, Channel_conn_callb{controller, connector});
        }
    };

    template <typename Scallb, typename Rcallb>
    class Basic_controller : public Sender_intf//, public Disconnector_intf
    {
        /* Low level implementation of mqtt cient */
        Impl::Controller controller;
        /* Current state */
        // bool started = false;

        /* Logic behind channels */
        std::shared_ptr<Validator> validator;
        std::shared_ptr<Connector> connector;
        // /* User callbacks */
        // std::shared_ptr<Sent_callb<Scallb>> sent_callb;
        // std::shared_ptr<Rec_callb<Rcallb>> rec_callb;
        // std::shared_ptr<Conn_callb> conn_callb;

        // virtual void write_handle(const std::string &channel_name, const Data &data, std::size_t write_len) = 0;

    public:
        Basic_controller(Server &server,
                         User_opt &user,
                         Scallb &&sent_msg,
                         Rcallb &&rec_msg);
        Basic_controller(Basic_controller &&) = default;
        Basic_controller &operator=(Basic_controller &&) = default;
        Basic_controller(const Basic_controller &) = delete;
        Basic_controller &operator=(const Basic_controller &) = delete;
        virtual ~Basic_controller() = default;

        void run();
        void write(const std::string &channel_name, const Data &data, std::size_t write_len) override final;
        virtual void disconnect(Time time);

        template <class E, typename S, typename... E_rgs>
        void add_channel(S &&channel_name, E_rgs &&...e_args);
    };

    template <typename Scallb, typename Rcallb>
    void Basic_controller<Scallb, Rcallb>::run()
    {
        // started = true;
        if (!connector->load_channels())
            throw std::logic_error("No channels to connect were specyfied!");

        controller.connect(Conn_callb{controller, connector});
    }

    template <typename Scallb, typename Rcallb>
    Basic_controller<Scallb, Rcallb>::Basic_controller(Server &server,
                                                       User_opt &user,
                                                       Scallb &&sent_msg,
                                                       Rcallb &&rec_msg)
        : validator{new Validator{}},
          connector{new Connector{validator}},
          controller{server, user}
        //   sent_callb{controller, std::move(sent_msg)},
        //   rec_callb{controller, std::move(rec_msg)},
        //   conn_callb{controller, connector}
    {
        controller.set_write_callb(Sent_callb<Scallb>{controller, std::move(sent_msg)});
        controller.set_read_callb(Rec_callb<Rcallb>{controller, validator, std::move(rec_msg)});
    }

    // template <typename Scallb, typename Rcallb>
    // void Basic_controller<Scallb, Rcallb>::read(const std::string &channel_name, std::vector<unsigned char> data)
    // {
    //     if (validator->validate(channel_name))
    //         validator->get_exec(channel_name).get()->exec(data);
    //     else
    //         throw std::logic_error("Somehow tried to write to channel that client was not connected to!");
    // }

    template <typename Scallb, typename Rcallb>
    void Basic_controller<Scallb, Rcallb>::write(const std::string &channel_name, const Data &data, std::size_t write_len)
    {
        if (validator->validate(channel_name))
        {
            controller.write(channel_name, data, write_len);
        }
        else
        {
            throw std::runtime_error("Tried to write to unknown channel " + channel_name + ".");
        }
    }

    template <typename Scallb, typename Rcallb>
    void Basic_controller<Scallb, Rcallb>::disconnect(Time time)
    {
        controller.disconnect(time);
    }

    template <typename Scallb, typename Rcallb>
    template <class E, typename S, typename... E_rgs>
    void Basic_controller<Scallb, Rcallb>::add_channel(S &&channel_name, E_rgs &&...e_args)
    {
        validator->add_channel<E>(std::forward<S>(channel_name), std::forward<E_rgs>(e_args)...);
    }

}