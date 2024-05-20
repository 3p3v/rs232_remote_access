#pragma once

#include <optional>
#include <string>
#include <Unauthed_ext.hpp>
#include <Basic_timer.hpp>
#include <Packet_defs.hpp>
#include <functional>

namespace Logic
{
    class Device : public Device_base
    {
    };

    /// @brief Used by tested object to send data to serial
    class Serial_side_impl
    {
        static std::vector<std::string> last_msg;

    public:
        static auto get_last_msg()
        {
            auto last_ = last_msg;
            last_msg.clear();
            return last_;
        }

        template <
            typename Iter_t,
            typename Ok_callb,
            typename Ec_callb>
        void write(
            Iter_t begin,
            Iter_t end,
            Ok_callb &&ok_callb,
            Ec_callb &&ec_callb)
        {
            last_msg.emplace_back(begin, end);
            ok_callb(5);
        }

        Serial_side_impl() = default;
        Serial_side_impl(Serial_side_impl&&) = default;
        Serial_side_impl& operator=(Serial_side_impl&&) = default;
        Serial_side_impl(const Serial_side_impl &) = default;
        Serial_side_impl& operator=(const Serial_side_impl &) = default;
        ~Serial_side_impl() = default;
    };

    std::vector<std::string> Serial_side_impl::last_msg = std::vector<std::string>();

    /// @brief Used by tested object to send data to remote
    class Remote_side_impl
    {
        using Id_t = std::decay_t<decltype(Packet_defs::min_msg_num)>;

        static std::vector<std::string> last_msg;
        static Id_t last_id;

    public:
        static auto get_last_msg()
        {
            auto last_ = last_msg;
            last_msg.clear();
            return last_;
        }

        static auto get_last_id()
        {
            return last_id;
        }

        template <
            typename Iter_t,
            typename Ok_callb,
            typename Ec_callb>
        void write(
            char id,
            Iter_t begin,
            Iter_t end,
            Ok_callb &&ok_callb,
            Ec_callb &&ec_callb)
        {
            last_id = id;
            last_msg.emplace_back(begin, end);
            ok_callb(5, 5);
        }

        Remote_side_impl() = default;
        Remote_side_impl(Remote_side_impl&&) = default;
        Remote_side_impl& operator=(Remote_side_impl&&) = default;
        Remote_side_impl(const Remote_side_impl &) = default;
        Remote_side_impl& operator=(const Remote_side_impl &) = default;
        ~Remote_side_impl() = default;
    };

    Remote_side_impl::Id_t Remote_side_impl::last_id{0};

    std::vector<std::string> Remote_side_impl::last_msg = std::vector<std::string>();

    /// @brief Used by serial to send data to tested object
    class Serial_side_conn_impl
    {
        /// @brief Current message number
        std::decay_t<decltype(Packet_defs::min_msg_num)> msg_num{Packet_defs::min_msg_num};

        /// @brief If callback was executed
        bool callb_run_;

        using Val_t = unsigned char;
        using Cont_t = Mqtt_msg<Val_t>::Cont_t;
        /// @brief Iterator to begin of message
        Cont_t::iterator begin;
        /// @brief Iterator to end of message
        Cont_t::iterator end;
        /// @brief Callback given by tested object
        std::function<void(
            Cont_t::iterator,
            Cont_t::iterator,
            std::function<void(Cont_t::iterator, Cont_t::iterator)> &)>
            callb;

    public:
        template <typename Str>
        void send(Str &&msg)
        {
            size_t msg_len;

            if constexpr (std::is_same_v<std::string, std::decay_t<Str>>)
                msg_len = std::size(msg);
            else
                msg_len = std::size(msg) - 1;

            if (msg_len > (end - begin))
                throw std::logic_error{""};

            auto b = std::begin(msg);

            std::copy(
                b,
                b + msg_len,
                begin);

            callb(
                begin,
                begin + msg_len,
                /* Callback to run after sending message */
                std::function<void(Cont_t::iterator, Cont_t::iterator)>{[this](Cont_t::iterator begin, Cont_t::iterator end)
                                                                        {
                                                                            this->begin = begin;
                                                                            this->end = end;
                                                                            callb_run_ = true;
                                                                        }});
        }

        auto if_callb_run()
        {
            auto cr = callb_run_;
            callb_run_ = false;
            return cr;
        }

        template <
            typename iter_t,
            typename Ok_callb,
            typename Ec_callb>
        void connect(
            iter_t begin,
            iter_t end,
            Ok_callb &&ok_callb,
            Ec_callb &&ec_callb)
        {
            this->begin = begin;
            this->end = end;
            callb = std::forward<Ok_callb>(ok_callb);
        }

        Serial_side_conn_impl() = default;
        Serial_side_conn_impl(Serial_side_conn_impl&&) = default;
        Serial_side_conn_impl& operator=(Serial_side_conn_impl&&) = default;
        Serial_side_conn_impl(const Serial_side_conn_impl &) = default;
        Serial_side_conn_impl& operator=(const Serial_side_conn_impl &) = default;
        ~Serial_side_conn_impl() = default;
    };

    /// @brief Used by remote to send data to tested object
    class Remote_side_conn_impl
    {
        /// @brief Current message number
        std::decay_t<decltype(Packet_defs::min_msg_num)> msg_num{Packet_defs::min_msg_num};

        /// @brief If callback was executed
        bool callb_run_;

        /// @brief Callback given by tested object
        std::function<void(
            std::decay_t<decltype(Packet_defs::min_msg_num)>,
            std::string::iterator,
            std::string::iterator,
            std::function<void()>)>
            callb;

        auto get_msg_num()
        {
            return msg_num++;
        }

    public:
        void set_msg_num(std::decay_t<decltype(Packet_defs::min_msg_num)> msg_num)
        {
            this->msg_num = msg_num;
        }

        template <typename Str>
        void send(Str &&msg)
        {
            auto msg_ptr = std::make_shared<std::string>(std::forward<Str>(msg));
            auto b = msg_ptr->begin();
            auto e = msg_ptr->end();

            callb(
                get_msg_num(),
                b,
                e,
                /* Callback to run after sending message */
                [msg_ptr = std::move(msg_ptr), this]()
                {
                    callb_run_ = true;
                });
        }

        auto if_callb_run()
        {
            auto cr = callb_run_;
            callb_run_ = false;
            return cr;
        }

        template <
            typename Ok_callb,
            typename Ec_callb>
        void connect(
            Ok_callb &&ok_callb,
            Ec_callb &&ec_callb)
        {
            callb = std::forward<Ok_callb>(ok_callb);
        }

        Remote_side_conn_impl() = default;
        Remote_side_conn_impl(Remote_side_conn_impl&&) = default;
        Remote_side_conn_impl& operator=(Remote_side_conn_impl&&) = default;
        Remote_side_conn_impl(const Remote_side_conn_impl &) = default;
        Remote_side_conn_impl& operator=(const Remote_side_conn_impl &) = default;
        ~Remote_side_conn_impl() = default;
    };

    class Remote_sett_impl
    {
        static std::vector<std::string> last_msg_s;
        static std::vector<std::string> last_msg_i;

    public:
        static auto get_last_msg_s()
        {
            auto last_ = last_msg_s;
            last_msg_s.clear();
            return last_;
        }

        static auto get_last_msg_i()
        {
            auto last_ = last_msg_i;
            last_msg_i.clear();
            return last_;
        }

        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            last_msg_i.push_back(msg + ' ' + arg);
        }

        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            last_msg_i.push_back(msg);
        }

        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            last_msg_s.push_back(msg + ' ' + arg);
        }

        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            last_msg_s.push_back(msg);
        }

        Remote_sett_impl() = default;
        Remote_sett_impl(Remote_sett_impl&&) = default;
        Remote_sett_impl& operator=(Remote_sett_impl&&) = default;
        Remote_sett_impl(const Remote_sett_impl &) = default;
        Remote_sett_impl& operator=(const Remote_sett_impl &) = default;
        ~Remote_sett_impl() = default;
    };

    std::vector<std::string> Remote_sett_impl::last_msg_s = std::vector<std::string>();
    std::vector<std::string> Remote_sett_impl::last_msg_i = std::vector<std::string>();

    template <typename Callb>
    class Custom_timer_impl : public Basic_timer
    {
        /* Callback */
        Callb callb;

        static Basic_timer *active;

    public:
        static void force_timeout()
        {
            active->callb();
        }

        void start() override
        {
        }

        void stop() override
        {
        }

        Custom_timer_impl(Callb &&callb, std::chrono::milliseconds interval = std::chrono::seconds{10})
            : Basic_timer{interval}, callb{std::move(callb)}
        {
            active = this;
        }

        Custom_timer_impl(Custom_timer_impl&&) = default;
        Custom_timer_impl& operator=(Custom_timer_impl&&) = default;

        ~Custom_timer_impl()
        {
            if (active == this)
                active = nullptr;
        }
    };

    template <typename Callb>
    Basic_timer *Custom_timer_impl<Callb>::active{nullptr};

    class Custom_timer_impl_maker
    {
    public:
        template <typename Callb>
        static auto make_timer(Callb &&callb, std::chrono::milliseconds interval = std::chrono::seconds{10})
        {
            return std::make_unique<Custom_timer_impl<std::decay_t<Callb>>>(std::forward<Callb>(callb), interval);
        }
    };

    class Observer
        : public Unauthed_ext
    {
    private:
        static Observer *active;

        bool if_get_set_param_fired_{false};
        bool if_param_ready_fired_{false};

    public:
        // static bool if_get_set_param_fired()
        // {
        //     auto temp = active->if_get_set_param_fired_;
        //     active->if_get_set_param_fired_ = false;

        //     return temp;
        // }

        static bool if_param_ready_fired()
        {
            auto temp = active->if_param_ready_fired_;
            active->if_param_ready_fired_ = false;

            return temp;
        }

    private:
        // /// @brief Observe if get_set_param_job was triggered
        // void add_get_set_param_job()
        // {
        //     add_handler(
        //         Job_type::Urgent,
        //         Job_policies<>::make_job_handler<Get_set_param_job>(
        //             [this](auto &job)
        //             {
        //                 if_get_set_param_fired_ = true;
        //             }));
        // }

        void add_param_ready_notify_job()
        {
            add_handler(
                Job_type::Urgent,
                Job_policies<>::make_job_handler<Param_ready_notify_job>(
                    [this](auto &job)
                    {
                        if_param_ready_fired_ = true;
                    }));
        }

    protected:
        /// @brief Observe if restart_job was triggered
        void add_restart_job() override
        {
        }

    public:
        static void restart_job_()
        {
            active->restart_job();
        }

        Cmds_pack get_cmds() override
        {
            return Cmds_pack{};
        }

        template <typename Dev_ptr_t>
        Observer(Forwarder &&manager, Notyfier &&n, Dev_ptr_t &&dev)
            : Unauthed_ext{std::move(manager), std::move(n), std::forward<Dev_ptr_t>(dev)}
        {
            active = this;

            // add_get_set_param_job();
            add_param_ready_notify_job();
        }

        Observer(Observer &&) = default;
        Observer &operator=(Observer &&) = default;
        Observer(const Observer &) = delete;
        Observer &operator=(const Observer &) = delete;

        ~Observer()
        {
            if (active == this)
                active = nullptr;
        }
    };

    Observer *Observer::active{nullptr};
}