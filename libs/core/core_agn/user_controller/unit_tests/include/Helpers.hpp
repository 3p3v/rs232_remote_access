#pragma once

#include <optional>
#include <string>
#include <Unauthed_ext.hpp>
#include <Basic_timer.hpp>
#include <Change_param_job.hpp>
#include <Device_base.hpp>

namespace Logic
{
    class Device : public Device_base
    {
    public: 
        Process_full manager{};
        Notyfication_manager nm{};
        Remote_dev rec{Remote_conf_port::Configurable};
    };

    class Observer : public Unauthed_ext
    {
    private:
        static Observer *active;

        bool if_change_param_job_fired_{false};

    public:
        static bool if_change_param_job_fired()
        {
            auto temp = active->if_change_param_job_fired_;
            active->if_change_param_job_fired_ = false;

            return temp;
        }

    private:
        /// @brief Observe if get_set_param_job was triggered
        void add_change_param_job()
        {
            add_handler(
                Job_type::Urgent,
                Job_policies<>::make_job_handler<Change_param_job>(
                    [this](auto &job)
                    {
                        if_change_param_job_fired_ = true;
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

        Cmds_pack return_cmds() override
        {
            return Cmds_pack{};
        }

        template <typename Dev_ptr_t>
        Observer(Forwarder &&manager, Notyfier &&n, Dev_ptr_t &&dev)
            : Unauthed_ext{std::move(manager), std::move(n), std::forward<Dev_ptr_t>(dev)}
        {
            active = this;

            add_change_param_job();
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