#pragma once

// #include <Base_ctrl_con_exec.hpp>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <Console_storage.hpp>
#include <Ext_storage.hpp>
#include <Remote_ext.hpp>
#include <Shared_ext.hpp>

namespace Logic
{
    /// @brief Storing and managing extensions; storing and running cmds.
    class Process_manager
        : private Console_storage,
          private Ext_storage
    {
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

    public:
        Process_manager() = default;
        Process_manager(Process_manager &&) = default;
        Process_manager &operator=(Process_manager &&) = default;
        Process_manager(const Process_manager &) = delete;
        Process_manager &operator=(const Process_manager &) = delete;
        ~Process_manager() = 0;

    protected:
        using Cont_t = Ext_cont::Cont_t;

    protected:
        /// @brief Storage for all exts.
        /// Many exts of same type are not allowed.
        Cont_t exts;

        /// @brief Function used to find iterator to extension of given type
        /// @tparam Remote_ext_t
        /// @tparam
        /// @return
        template <
            typename Remote_ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        Cont_t::iterator find_ext()
        {
            auto type = typeid(Remote_ext_t);

            auto itr = std::find_if(
                exts.begin(),
                exts.end(),
                [&, this](auto &e)
                {
                    if (type == typeid(*e))
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
            add_ext(Shared_ext(std::make_shared<Remote_ext_t>(std::forward<Args_t>(args)...)));
        }

        template <
            typename Remote_ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        void add_ext(Remote_ext_t &&ext)
        {
            add_ext(Shared_ext(std::forward<Remote_ext_t>(ext)));
        }

        void add_ext(const Shared_ext &ext_ptr);

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
                    remove_cmd(rec.first)
                });
            /* Remove worker refs */
            remove_worker(ext.get_ptr());
            /* Remove worker from manager */
            exts.erase(ext_itr);
        }
    };
}