#pragma once

#include <optional>
#include <string>
#include <Unauthed_ext.hpp>
#include <Basic_timer.hpp>
// #include <Serial_base.hpp>

namespace Logic
{
    class Device : public Device_base
    {

    };

    class Serial_sett_impl
    {
    public:
        static unsigned int baud_rate;
        static Port_settings::Parity parity;
        static unsigned int char_size;
        static Port_settings::Stop_bits stop_bits;

    public:
        void set_baud_rate(unsigned int baud_rate)
        {
            this->baud_rate = baud_rate;
        }

        void set_parity(const Port_settings::Parity parity)
        {
            this->parity = parity;
        }

        void set_char_size(unsigned int char_size)
        {
            this->char_size = char_size;
        }

        void set_stop_bits(const Port_settings::Stop_bits stop_bits)
        {
            this->stop_bits = stop_bits;
        }

        Serial_sett_impl()
        {

        }
    };

    unsigned int Serial_sett_impl::baud_rate = 0;
    Port_settings::Parity Serial_sett_impl::parity = Port_settings::Parity::odd;
    unsigned int Serial_sett_impl::char_size = 0;
    Port_settings::Stop_bits Serial_sett_impl::stop_bits = Port_settings::Stop_bits::two;
    
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
    };

    std::vector<std::string> Remote_sett_impl::last_msg_s = std::vector<std::string>();
    std::vector<std::string> Remote_sett_impl::last_msg_i = std::vector<std::string>();

    template <typename Callb>
    class Custom_timer_impl : public Basic_timer
    {
        /* Callback */
        Callb callb;

        static Basic_timer* active;

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

        ~Custom_timer_impl()
        {
            if (active == this)
                active = nullptr;
        }
    };

    template <typename Callb>
    Basic_timer* Custom_timer_impl<Callb>::active{nullptr};

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
        static Observer* active;

        bool if_get_set_param_fired_{false};
        bool if_param_ready_fired_{false};

    public:
        static bool if_get_set_param_fired()
        {
            auto temp = active->if_get_set_param_fired_;
            active->if_get_set_param_fired_ = false;

            return temp;
        }

        static bool if_param_ready_fired()
        {
            auto temp = active->if_param_ready_fired_;
            active->if_param_ready_fired_ = false;

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

            add_get_set_param_job();
            add_param_ready_notify_job();
        }

        Observer(Observer&&) = default;
        Observer& operator=(Observer&&) = default;
        Observer(const Observer&) = delete;
        Observer& operator=(const Observer&) = delete;

        ~Observer()
        {
            if (active == this)
                active = nullptr;
        }
    };

    Observer* Observer::active{nullptr};
}