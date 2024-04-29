#pragma once

// #include <Base_ctrl_con_exec.hpp>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <Ctrl_con_storage.hpp>
#include <Forwarder.hpp>
#include <Storage.hpp>
#include <Remote_ext.hpp>

using namespace Cmd_ctrl;
using namespace Job_ctrl;

namespace Logic
{
    /// @brief Way of referencing workers
    using Worker_ref = Worker&;
    using Ext_forwarder = Forwarder<Worker_ref>;
    using Ext_storage = Storage<Worker_ref>;

    /// @brief Argument types for received command
    using Con_storage_param = Remote_ext_base::Command;
    using Console_storage = Ctrl_con_storage<Con_storage_param, Endl_opt::with>;

    /// @brief Basic needs of Logic extension
    class Process_manager
        : private Console_storage,
          private Ext_storage,
          public Ext_forwarder
    {
    protected:
        using Ext_ptr = std::shared_ptr<Worker>;
        using Ext_cont_t = std::unordered_set<Ext_ptr>;

        /// @brief Storage for all exts.
        /// Many exts of same type are not allowed.
        Ext_cont_t exts;

        /// @brief Function used to find iterator to extension of given type
        /// @tparam Remote_ext_t 
        /// @tparam  
        /// @return 
        template <
            typename Remote_ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        Ext_cont_t::iterator find_ext()
        {
            auto type = typeid(Remote_ext_t);

            auto itr = std::find_if(
                exts.begin(),
                exts.end(),
                [&, this](auto &e)
                {
                    if (type == typeid(e))
                        return true;
                    else 
                        return false;
                }
            );

            if (itr == exts.end())
                throw std::logic_error{"Extension was not found so it can not be removed!"};
            
            return itr;
        }

    public:
        /// @brief Function used to initialize and add extension
        /// @tparam Remote_ext_t 
        /// @tparam ...Args_t 
        /// @tparam  
        /// @param ...ext 
        template <
            typename Remote_ext_t,
            typename... Args_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        void add_ext(Args_t &&...ext)
        {
            add_ext(std::make_shared<Remote_ext_t>(std::forward<Args_t>(args)...));
        }

        /// @brief Function used to add already initialized extension pointer 
        /// @tparam Remote_ext_t 
        /// @tparam  
        /// @param ext_ptr 
        template <
            typename Remote_ext_ptr_t,
            std::enable_if_t<Process_manager::is_shared_ptr<std::decay_t<Remote_ext_ptr_t>>::value, bool> = true>
        void add_ext(Remote_ext_ptr_t &&ext_ptr)
        {           
            /* Commands for communcation between extension and device */
            auto cmds = ext_ptr->get_cmds();
            std::for_each(
                cmds.begin(),
                cmds.end(),
                [this](Ctrl_cmd_pair &cmd)
                {
                    add_cmd(std::move(cmd));
                });

            /* Commands for communcation between extensions */
            add_worker(*ext_ptr);

            /* Add extension ptr (above use references to not make too many ptrs) */
            exts.push_back(std::forward<Remote_ext_ptr_t>(ext_ptr));
        }

        /// @brief Collective function for removing extensions
        /// @tparam Remote_ext_t 
        /// @tparam  
        template<
            typename Remote_ext_t,
            std::enable_if_t<std::is_base_of_v<Remote_ext_base, std::decay_t<Remote_ext_t>>, bool> = true>
        void remove_ext()
        {
            auto ext_itr = find_ext<Remote_ext_t>();
            auto &ext = *ext_itr;

            /* Remove commands associated with worker */
            remove_cmd(ext);
            /* Remove worker refs */
            remove_worker(ext);
            /* Remove worker from manager */
            exts.erase(ext_itr);
        }
    };
}