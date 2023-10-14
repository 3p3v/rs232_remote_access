#pragma once

#include <memory>
#include <stdexcept>
#include <Sender_intf.hpp>
#include <Receiver.hpp>
#include <Sender.hpp>
#include <Connector.hpp>
#include <Def.hpp>
// #include <Traffic_channel_connector.hpp>

namespace Mqtt_port
{

    class Basic_controller : public Sender_intf
    {
    protected:
        /* Current state */
        bool started = false;

        /* Logic behind channels */
        std::shared_ptr<Validator> validator;
        std::shared_ptr<Receiver> receiver;
        std::shared_ptr<Connector> connector;

        virtual void run_handle() = 0;
        virtual void write_handle(const std::string &channel_name, const Data &data, std::size_t write_len) = 0;

    public:
        Basic_controller();
        Basic_controller(Basic_controller &&) = default;
        Basic_controller &operator=(Basic_controller &&) = default;
        Basic_controller(const Basic_controller &) = delete;
        Basic_controller &operator=(const Basic_controller &) = delete;
        virtual ~Basic_controller() = default;

        void run();
        void write(const std::string &channel_name, const Data &data, std::size_t write_len) override final;

        template <typename S, class E>
        void add_channel(S &&channel_name, E &&executor)
        {
            validator->add_channel(std::forward<S>(channel_name), std::forward<E>(executor));
        }
    };

}