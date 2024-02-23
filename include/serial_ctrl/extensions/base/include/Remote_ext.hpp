#pragma once

#include <stdexcept>
#include <Forwarder.hpp>
#include <Restart_job.hpp>

using namespace Job_ctrl;
using namespace Cmd_ctrl;

namespace Logic
{
    class Remote_ext_base
    {
    public:
        /// @brief Type of parameter used by all commands
        using Cmd_param = const std::string &;
        /// @brief Type of command used by extensions
        using Cmd = Exec<>::Param<Cmd_param>::Base_handle_intf;
        using Cmds_pack = std::vector<Cmd>;

        /// @brief Get all commands that extension uses to communicate with remote
        /// @return
        virtual Cmds_pack get_cmds() = 0;

        Remote_ext_base() = default;
        Remote_ext_base(const Remote_ext_base &) = delete;
        Remote_ext_base &operator=(const Remote_ext_base &) = delete;
        Remote_ext_base(Remote_ext_base &&) = default;
        Remote_ext_base &operator=(Remote_ext_base &&) = default;
        virtual ~Remote_ext_base() = 0;
    };
    
    /// @brief Base functionality of any module implementing functionality of MQTT<->Serial communication
    template <typename Manager_t>
    class Remote_ext : public Worker, public Remote_ext_base
    {
        template <typename T>
        struct is_shared_ptr : std::false_type
        {
        };
        template <typename T>
        struct is_shared_ptr<std::shared_ptr<T>> : std::true_type
        {
        };
        template <typename T>
        struct is_weak_ptr : std::false_type
        {
        };
        template <typename T>
        struct is_weak_ptr<std::weak_ptr<T>> : std::true_type
        {
        };

    public:
        using Manager_ptr = std::weak_ptr<Manager_t>;

    protected:
        Manager_ptr manager;

        template <
            typename Job_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    std::decay_t<Job_t>>>>
        void forward_job(Job_t &&job);

        /// @brief Restart module procedure
        virtual void add_restart_job() = 0;

        /// @brief Send signal to restart all modules
        void restart_job();

        /// @brief Default error callback
        /// @return
        auto def_ec_callb();

    public:
        template <
            typename Manager_ptr_t,
            typename = std::enable_if_t<
                is_shared_ptr<Manager_ptr_t> ||
                is_weak_ptr<Manager_ptr_t>>>
        Remote_ext(Manager_ptr_t &&manager);
        ;
        Remote_ext(const Remote_ext &) = delete;
        Remote_ext &operator=(const Remote_ext &) = delete;
        Remote_ext(Remote_ext &&) = default;
        Remote_ext &operator=(Remote_ext &&) = default;
        virtual ~Remote_ext() = 0;
    };

    template <typename Manager_t>
    inline void Remote_ext<Manager_t>::restart_job()
    {
        forward_job(Restart_job{});
    }

    template <typename Manager_t>
    inline auto Remote_ext<Manager_t>::def_ec_callb()
    {
        return [](const std::exception &e) {};
    }

    template <typename Manager_t>
    inline Remote_ext<Manager_t>::~Remote_ext() = default;

    template <typename Manager_t>
    template <typename Job_t, typename>
    inline void Remote_ext<Manager_t>::forward_job(Job_t &&job)
    {
        if (auto manager_ = manager.lock())
        {
            manager_->forward_job(std::forward<Job_t>(job));
        }
    }

    template <typename Manager_t>
    template <typename Manager_ptr_t, typename>
    inline Remote_ext<Manager_t>::Remote_ext(Manager_ptr_t &&manager)
        : manager{std::forward<Manager_ptr_t>(manager)}
    {
        add_restart_job();
    }
}