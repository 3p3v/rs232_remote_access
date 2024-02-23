#pragma once

#include <optional>
#include <string>
#include <Unauthed_ext.hpp>
#include <Basic_timer.hpp>

namespace Logic
{
class Remote_sett_impl
    {
        static std::string last_msg_s;
        static std::string last_msg_i;

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
            last_msg_i = msg + ' ' + arg;
        }

        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            last_msg_i = msg;
        }

        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            last_msg_s = msg + ' ' + arg;
        }

        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        inline void write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
            last_msg_s = msg;
        }
    };

    std::string Remote_sett_impl::last_msg_s{};
    std::string Remote_sett_impl::last_msg_i{};

    template <typename Callb>
    class Custom_timer_impl : public Basic_timer
    {
        /* Callback */
        Callb callb;

        std::optional<Basic_timer &> active{std::nullopt};

    public:
        static size_t timer_counter{};

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

        Custom_timer(Callb &&callb)
            : callb{std::move(callb)}
        {
            active.reset(*this)
        }

        ~Custom_timer()
        {
        }
    };

    class Custom_timer_impl_maker
    {
    public:
        template <typename Callb>
        auto make_timer(Callb &&callb, std::chrono::milliseconds interval = std::chrono::seconds{10})
        {
            return std::make_unique<Custom_timer_impl<Callb>>(std::forward<Callb>(callb), interval);
        }
    };

    class Observer : public Unauthed_ext
    {
    private:
        static std::optional<Observer &> active;

        bool if_get_set_param_fired_{false};

    public:
        static bool if_get_set_param_fired()
        {
            auto temp = active->if_get_set_param_fired_;
            active->if_get_set_param_fired_ = false;

            return temp;
        }

    private:
        /// @brief Observe if get_set_param_job was triggered
        void add_get_set_param_job()
        {
            add_handler(
                Job_type::Urgent,
                Job_policies<>::make_job_handler<Get_set_param_job>(
                    [this](auto &job)
                    {
                        if_get_set_param_fired_ = true;
                    }));
        }

    protected:
        /// @brief Observe if restart_job was triggered
        void add_restart_job() override
        {
        }

    public:
        /// @brief Inform that parameters are ready
        static void param_ready_notify_job()
        {
            active->forward_job(Param_ready_notify_job{});
        }

        /// @brief Inform that parameters were changed
        static void param_change_notify_job()
        {
            active->forward_job(Param_change_notify_job{});
        }

        Cmds_pack get_cmds() override
        {
            return Cmds_pack{};
        }

        template <typename Forwarder_ptr_t>
        Observer(Forwarder_ptr_t &&manager)
            : Unauthed_ext{std::forward<Forwarder_ptr_t>(manager)}
        {
            active.reset(*this);

            add_get_set_param_job();
        }
    };

    std::optional<Observer &> Observer::active{std::nullopt};
}