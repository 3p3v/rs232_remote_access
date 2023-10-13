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
    class Callbacks
    {
        // using Data = std::vector<unsigned int>;
    public:
        /* Callbacks */
        virtual void write_call(const std::string &channel_name, std::size_t write_len) = 0;
        virtual void read_call(const Data &begin, std::size_t write_len) = 0;
        virtual void write_err_call(const int code) = 0;
        virtual void read_err_call(const int code) = 0;
        // virtual void conn_call(const std::string& channel_name) = 0;
        virtual void conn_call() = 0;
        // virtual void conn_err_call(const std::string& channel_name) = 0;
        virtual void conn_err_call() = 0;
    };

    // template <>
    class Basic_controller : public Receiver, public Sender, public Connector//, public Callbacks// : public Traffic_receiver, public Traffic_sender, public Traffic_channel_connector
    {
    protected:
        /* Current state */
        bool started = false;

        /* Logic behind channels */
        std::shared_ptr<Validator> validator;

        std::unique_ptr<Callbacks> callb;

        virtual void run_exec() = 0;

    public:
        template <typename Callb_t>
        Basic_controller(std::shared_ptr<Validator> validator,
                         Callb_t &&callb);
        Basic_controller(Basic_controller &&) = default;
        Basic_controller &operator=(Basic_controller &&) = default;
        Basic_controller(const Basic_controller &) = delete;
        Basic_controller &operator=(const Basic_controller &) = delete;
        virtual ~Basic_controller() = default;

        void run();
        // void write(const std::string &channel_name, const Data &data, std::size_t write_len) override final;
        // template <typename S, class E>
        // void add_channel(S &&channel_name, E &&executor);
    };

    template <typename Callb_t>
    Basic_controller::Basic_controller(std::shared_ptr<Validator> validator,
                                       Callb_t &&callb)
        : Receiver{validator}, 
          Sender{validator}, 
          Connector{validator},
          validator{validator},
          callb{std::forward<Callb_t>(callb)}
    {

    }
    
    // template <typename Call_t>
    // Basic_controller<Call_t>::Basic_controller()
    //     : Receiver{validator}, 
    //       Sender{validator}, 
    //       Connector{validator}
    // {
    // }

    
    // // void Basic_controller::write(const std::string &channel_name, const Data &data, std::size_t write_len)
    // // {
    // //     sender.write(channel_name, data, write_len);
    // // }

    // template <typename Call_t>
    // void Basic_controller<Call_t>::run()
    // {
    //     started = true;
    //     load_channels();
    //     run_exec();
    // }

    
    // template <typename S, class E>
    // void Basic_controller::add_channel(S &&channel_name, E &&executor)
    // {
    //     if (!started)
    //         validator.add_channel(channel_name, executor);
    //     else
    //         throw std::logic_error("Tried to add channel while connection was already in running state!");
    // }
}