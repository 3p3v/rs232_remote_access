#pragma once

#include <memory>
#include <stdexcept>
#include <impl/Controller.hpp>
#include <Validator.hpp>
#include <Connector.hpp>
#include <Channel_conn_callb.hpp>
#include <Conn_callb.hpp>
#include <Rec_callb.hpp>
#include <Sent_callb.hpp>

namespace Mqtt_port
{
    class Controller final : protected Callbacks_defs
    {
        /* Low level implementation of mqtt cient */
        Impl::Controller controller;

        /* Logic behind channels */
        std::shared_ptr<Validator> validator;
        std::shared_ptr<Connector> connector;

    public:
        template <typename Scallb, typename Rcallb>
        Controller(Server::Get_cont &&server,
                   User::Get_cont &&user,
                   Scallb &&sent_msg,
                   Rcallb &&rec_msg);
        Controller(Controller &&) = default;
        Controller &operator=(Controller &&) = default;
        Controller(const Controller &) = delete;
        Controller &operator=(const Controller &) = delete;
        ~Controller() = default;

        void run();
        template <typename Iter>
        void write(const std::string &channel_name, const Iter begin, const Iter end);
        void disconnect(Time time);
        void connect_channels();

        // template <class E, typename S, typename... E_rgs>
        // void add_channel(S &&channel_name, E_rgs &&...e_args);

        template <class E, typename S>
        Controller& add_channel(S &&channel_name, E &&handler);
    };

    template <typename Scallb, typename Rcallb>
    Controller::Controller(Server::Get_cont &&server,
                           User::Get_cont &&user,
                           Scallb &&sent_msg,
                           Rcallb &&rec_msg)
        : validator{new Validator{}},
          connector{new Connector{}},
          controller{std::move(server), std::move(user)}
    {
        controller.set_write_callb(Sent_callb<Scallb>{controller, std::forward<Scallb>(sent_msg)});
        controller.set_read_callb(Rec_callb<Impl::Controller::Data, Rcallb>{controller, validator, std::forward<Rcallb>(rec_msg)});
    }

    template <typename Iter>
    void Controller::write(const std::string &channel_name, const Iter begin, const Iter end)
    {
        if (validator->validate(channel_name))
        {
            controller.write(channel_name, begin, end);
        }
        else
        {
            throw std::runtime_error("Tried to write to unknown channel " + channel_name + ".");
        }
    }

    // template <class E, typename S, typename... E_rgs>
    // void Controller::add_channel(S &&channel_name, E_rgs &&...e_args)
    // {
    //     validator->add_channel<E>(std::forward<S>(channel_name), std::forward<E_rgs>(e_args)...);
    // }

    template <class E, typename S>
    Controller& Controller::add_channel(S &&channel_name, E &&handler)
    {
        validator->add_channel<E>(S{channel_name}, std::forward<E>(handler));
        connector->load_channel(std::forward<S>(channel_name));

        return *this;
    }

}