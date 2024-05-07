#pragma once

#include <stdexcept>
#include <memory>
#include <Worker.hpp>
#include <Ext_forwarder.hpp>

// namespace Logic
// {
//     class Ext_forwarder;
// }

using namespace Job_ctrl;
using namespace Cmd_ctrl;

namespace Logic
{
    /// @brief Base functionality of any module implementing functionality of MQTT<->Serial communication
    class Remote_ext : public Worker
    {
    protected:
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

    protected:
        template <typename Str_t, typename Cmd_t>
        auto make_pack_elem(Str_t &&name, Cmd_t &&cmd);

    public:
        /// @brief Type of parameter used by all commands
        using Cmd_param = const std::string &;
        /// @brief Type of command used by extensions
        using Cmd = Exec<>::Param<Cmd_param>::Base_handle_intf;
        using Command = Exec<>::Param<Cmd_param>;
        using Cmds_pack = std::vector<std::pair<std::string, std::unique_ptr<Cmd>>>;

        /// @brief Get all commands that extension uses to communicate with remote
        /// @return
        virtual Cmds_pack get_cmds() = 0;

    public:
        using Forwarder_ptr = std::weak_ptr<Ext_forwarder>;

    protected:
        Forwarder_ptr forwarder;

        template <
            typename Job_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Job,
                    std::decay_t<Job_t>>>>
        void forward_job(Job_t &&job);

        /// @brief Send signal to restart all modules
        void restart_job();

        /// @brief Default error callback
        /// @return
        auto def_ec_callb();

        /// @brief Restart module procedure
        virtual void add_restart_job() = 0;

    public:
        template <typename Forwarder_ptr_t>
        Remote_ext(Forwarder_ptr_t &&forwarder);
        Remote_ext(const Remote_ext &) = delete;
        Remote_ext &operator=(const Remote_ext &) = delete;
        Remote_ext(Remote_ext &&) = default;
        Remote_ext &operator=(Remote_ext &&) = default;
        virtual ~Remote_ext() = 0;
    };

    using Remote_ext_base = Remote_ext;

    template <typename Job_t, typename>
    void Remote_ext::forward_job(Job_t &&job)
    {
        if (auto forwarder_ = forwarder.lock())
        {
            forwarder_->forward_job(std::forward<Job_t>(job));
        }
    }

    template <typename Forwarder_ptr_t>
    Remote_ext::Remote_ext(Forwarder_ptr_t &&forwarder)
        : forwarder{std::forward<Forwarder_ptr_t>(forwarder)}
    {
    }

    template <typename Str_t, typename Cmd_t>
    inline auto Remote_ext_base::make_pack_elem(Str_t &&name, Cmd_t &&cmd)
    {
        return std::make_pair(std::forward<Str_t>(name), std::make_unique<Cmd_t>(std::forward<Cmd_t>(cmd)));
    }

    inline auto Remote_ext::def_ec_callb()
    {
        return [](const std::exception &e) {};
    }
}