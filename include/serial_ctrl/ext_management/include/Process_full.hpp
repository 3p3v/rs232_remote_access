#pragma once

#include <memory>
#include <unordered_set>
#include <algorithm>
/* Command management */
#include <Ctrl_con_exec.hpp>
#include <Ctrl_con_storage.hpp>
/* Extension management */
#include <Forwarder.hpp>
#include <Storage.hpp>
#include <Remote_ext.hpp>
#include <Shared_ext.hpp>
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
    private:
        /// @brief Defines exts container 
        class Ext_cont
        {
        public:
            using Elem_t = Shared_ext;

        private:
            /// @brief Hash only based on type, can't insert two elements of same Extension type
            class Hash
            {
            public:
                Elem_t::Id_t operator()(const Elem_t &ext) const noexcept;
            };

            class KeyEqual
            {
            public:
                bool operator()(const Elem_t &ext1, const Elem_t &ext2) const noexcept;
            };

        public:
            Ext_cont() = delete;

            using Cont_t = std::unordered_set<
                Elem_t, 
                Hash, 
                KeyEqual>;
        };

        using Cont_t = Ext_cont::Cont_t;

        /// @brief Storage for all exts.
        /// Many exts of same type are not allowed.
        Cont_t exts = Cont_t{};

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

            auto itr = std::find_if(
                exts.begin(),
                exts.end(),
                [&, this](auto &e)
                {
                    if (type == e.id)
                        return true;
                    else
                        return false;
                });

            if (itr == exts.end())
                throw std::logic_error{"Extension was not found so it can not be removed!"};

            return itr;
        }

    public:
        /// @brief Function used to initialize and add extension
        /// @tparam Remote_ext_t
        /// @tparam ...Args_t
        /// @tparam
        /// @param ...args
        template <
            typename Remote_ext_t,
            typename... Args_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        void add_ext(Args_t &&...args)
        {
            add_ext(Shared_ext{Remote_ext_t{std::forward<Args_t>(args)...}});
        }

        // template <
        //     typename Remote_ext_t, 
        //     typename = std::enable_if_t<std::is_base_of_v<Remote_ext, std::decay_t<Remote_ext_t>>>>
        // void add_ext(Remote_ext_t &&ext);
        
        void add_ext(Shared_ext &&ext_ptr);

        /// @brief Collective function for removing extensions
        /// @tparam Remote_ext_t
        /// @tparam
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