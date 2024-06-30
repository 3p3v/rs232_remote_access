#pragma once

#include <memory>
#include <unordered_map>
#include <algorithm>
/* Command management */
#include <Ctrl_con_exec.hpp>
#include <Ctrl_con_storage.hpp>
/* Extension management */
#include <Forwarder.hpp>
#include <Storage.hpp>
#include <Remote_ext.hpp>
#include <Ext_id.hpp>
/* Objects needed for classes above */
#include <Ws_holder.hpp>

namespace Logic
{
    /// @brief Storing and managing extensions; storing and running cmds.
    class Process_full : private Ws_holder
    {
        /* ********************* Command control and jobs messaging ********************* */
        using Console_storage = Cmd_ctrl::Ctrl_con_storage<Con_storage_param, Endl_opt::with>;
        using Console_exec = Cmd_ctrl::Ctrl_con_exec<Con_storage_param, Endl_opt::with>;

        Console_storage cmd_storage{cc};

    public:
        Console_exec cmd_exec{cc};

    private:
        Storage ext_storage{ws};

    public:
        Forwarder ext_forwarder{ws};

        /* ********************* Extensions container ********************* */
    public:
        using Elem_t = std::unique_ptr<Remote_ext>;

    private:
        using Cont_t = std::unordered_map<Ext_id_base::Id_t, Elem_t>;

        /// @brief Storage for all exts.
        /// Many exts of same type are not allowed.
        Cont_t exts = Cont_t();

    public:
        Process_full() = default;
        Process_full(Process_full &&) = default;
        Process_full &operator=(Process_full &&) = default;
        Process_full(const Process_full &) = delete;
        Process_full &operator=(const Process_full &) = delete;
        ~Process_full() = default;

    private:
        /// @brief Function used to find iterator to extension of given type
        /// @tparam Remote_ext_t
        /// @tparam
        /// @return
        template <
            typename Remote_ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        Cont_t::iterator find_ext()
        {
            auto type = Ext_id<Remote_ext_t>::id;

            auto itr = exts.find(type);

            if (itr == exts.end())
                throw std::logic_error{"Extension was not found so it can not be removed!"};

            return itr;
        }

    public:
        /// @brief Construct and add extension
        /// @tparam Remote_ext_t Extension type
        /// @tparam ...Args_t
        /// @param ...args Arguments passed to Extension constructor
        template <
            typename Remote_ext_t,
            typename... Args_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        void add_ext(Args_t &&...args)
        {
            add_ext(std::make_unique<Remote_ext_t>(std::forward<Args_t>(args)...));
        }

        /// @brief Add extension
        /// @tparam Remote_ext_t 
        /// @tparam Extension type
        /// @param ext_ptr Extension in unique_ptr
        template <
            typename Remote_ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        void add_ext(std::unique_ptr<Remote_ext_t> &&ext_ptr)
        {
            auto &ext = *ext_ptr;

            /* Add extension ptr */
            if (exts.emplace(Ext_id<Remote_ext_t>::id, std::move(ext_ptr)).second == false)
                throw std::logic_error{"Extension of this type is already in use!"};

            /* Commands for communcation between extension and device */
            auto cmds = ext.get_cmds();
            std::for_each(
                cmds.begin(),
                cmds.end(),
                [this](auto &cmd)
                {
                    cmd_storage.add_cmd(std::move(cmd));
                });

            /* Commands for communcation between extensions */
            ext_storage.add_worker(ext);
        }

        /// @brief Collective function for removing extensions
        /// @tparam Remote_ext_t
        /// @tparam Type of extension to remove
        template <
            typename Remote_ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        void remove_ext()
        {
            auto ext_itr = find_ext<Remote_ext_t>();
            auto &ext = *ext_itr;

            /* Remove commands associated with worker */
            auto cmds = ext->get_cmds();
            std::for_each(
                cmds.begin(),
                cmds.end(),
                [](const auto &rec)
                {
                    cmd_storage.remove_cmd(rec.first)
                });
            /* Remove worker refs */
            ext_storage.remove_worker(*ext);
            /* Remove worker from manager */
            exts.erase(ext_itr);
        }
    };

    using Process_manager = Process_full;

    // template <typename Remote_ext_t, typename>
    // inline void Process_full::add_ext(Remote_ext_t &&ext)
    // {
    //     add_ext(Shared_ext{std::forward<Remote_ext_t>(ext)});
    // }
}