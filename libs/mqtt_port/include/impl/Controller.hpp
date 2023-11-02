#pragma once

#include <string>
#include <atomic>
#include <set>
#include <Def.hpp>
#include <User.hpp>
#include <Server.hpp>
#include <I_callb.hpp>
#include <O_callb.hpp>
#include <C_callb.hpp>
#include <Send_callb_impl.hpp>
#include <Conn_callb_impl.hpp>
#include <Channel_conn_callb_impl.hpp>
#include <mqtt/async_client.h>

namespace Mqtt_port
{
    namespace Impl
    {
        class Controller final : public virtual mqtt::callback
        {
            std::atomic_bool is_conn{false};
            std::vector<std::pair<std::string, std::string>> delayed_pub;
            std::vector<std::pair<std::string, unsigned char>> delayed_sub;

        public:
            using Data = std::string;

        protected:
            std::shared_ptr<mqtt::async_client> client;
            mqtt::connect_options options;

            /* Impl specyfic callbacks */
            std::unique_ptr<O_callb> sent_callb;
            std::unique_ptr<I_callb<Data>> rec_callb;
            std::unique_ptr<C_callb> conn_callb;
            std::unique_ptr<C_callb> channel_conn_callb;
            Send_callb_impl send_callb_impl{sent_callb};
            Conn_callb_impl conn_callb_impl{conn_callb};
            Channel_conn_callb_impl ch_conn_callb_impl{channel_conn_callb};

            void connected(const std::string & /*cause*/) override;

            void connection_lost(const std::string & /*cause*/) override;

            void message_arrived(mqtt::const_message_ptr msg) override;

            // disabled, becaurse doesn't work
            void delivery_complete(mqtt::delivery_token_ptr token) override;

        public:
            Controller(Server::Get_cont &&server, User::Get_cont &&user);
            Controller(Controller &&) = default;
            Controller &operator=(Controller &&) = default;
            Controller(Controller &) = delete;
            Controller &operator=(Controller &) = delete;
            ~Controller() = default;

            template <typename Callb_t>
            void connect(Callb_t &&conn_callb);

            template <typename Callb_t>
            void subscribe(const std::string &channel_name, unsigned char qos, Callb_t &&channel_conn_callb);

            void subscribe(const std::string &channel_name, unsigned char qos);

            void disconnect(Time time);

            /* Write data, use previous callback */
            template <typename Iter>
            void write(const std::string &channel_name, const Iter begin, const Iter end);

            /* Set callback and write data */
            template <typename Iter, typename Callb_t>
            void write(const std::string &channel_name, const Iter begin, const Iter end, Callb_t &&sent_callb);

            /* Set write callack */
            template <typename Callb_t>
            void set_write_callb(Callb_t &&sent_callb);

            /* Set read callack */
            template <typename Callb_t>
            void set_read_callb(Callb_t &&rec_callb);
        };

        template <typename Callb_t>
        void Controller::connect(Callb_t &&conn_callb)
        {
            /* Change callback */
            this->conn_callb.reset(new Callb_t{std::move(conn_callb)});

            /* Connect to broker */
            client->connect(options, nullptr, conn_callb_impl);
        }

        template <typename Callb_t>
        void Controller::subscribe(const std::string &channel_name, unsigned char qos, Callb_t &&channel_conn_callb)
        {
            /* Change callback */
            this->channel_conn_callb.reset(new Callb_t{std::move(channel_conn_callb)});

            /* Subscribe */
            subscribe(channel_name, qos);
        }

        /* Write data, use previous callback */
        template <typename Iter>
        void Controller::write(const std::string &channel_name, const Iter begin, const Iter end)
        {
            if (!is_conn)
            {
                delayed_pub.emplace_back(channel_name, std::string{begin, end});
            }
            else
            {
                /* Send message */
                auto msg = mqtt::make_message(channel_name, &(*begin), end - begin);
                client.get()->publish(msg, nullptr, send_callb_impl);

                // TODO delete, temporarly moved here
                sent_callb->success(channel_name,
                                    end - begin);
            }
        }

        /* Set callback and write data */
        template <typename Iter, typename Callb_t>
        void Controller::write(const std::string &channel_name, const Iter begin, const Iter end, Callb_t &&sent_callb)
        {
            /* Change callback */
            this->sent_callb.reset(new Callb_t{std::move(sent_callb)});

            /* Send message */
            write(channel_name, begin, end);
        }

        /* Set write callack */
        template <typename Callb_t>
        void Controller::set_write_callb(Callb_t &&sent_callb)
        {
            /* Change callback */
            this->sent_callb.reset(new Callb_t{std::move(sent_callb)});
        }

        /* Set read callack */
        template <typename Callb_t>
        void Controller::set_read_callb(Callb_t &&rec_callb)
        {
            /* Change callback */
            this->rec_callb.reset(new Callb_t{std::move(rec_callb)});
        }
    }

}