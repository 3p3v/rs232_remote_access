#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <stdexcept>

namespace Cmd_ctrl
{
    class Policy
    {
    public:
        virtual bool validate(const std::string &arg) = 0;
    };

    class NumbersOnly : public Policy
    {
    public:
        bool validate(const std::string &arg) override
        {
            return true;
        }
    };

    class Base_cmd_handle
    {
    protected:
        using Data = std::vector<unsigned char>;
        using Policy_ptr = std::unique_ptr<Policy>;
        using Policy_cont = std::vector<Policy_ptr>;
        Policy_cont policies;

    public:
        Base_cmd_handle(Policy_cont &&policies)
            : policies(std::move(policies))
        {
        }

        bool validate(const std::string &arg)
        {
            std::for_each(policies.begin(), policies.end(),
                          [&arg](Policy_ptr &policy)
                          {
                              policy->validate(arg);
                          });
        }

        virtual void exec(const Data &arg) = 0;
    };

    // template <typename... Policies>
    // class Cmd_handle_pol : public Base_cmd_handle
    // {
    // public:
    //     Cmd_handle_pol()
    //         : Base_cmd_handle{std::unique_ptr<Policy>{Policies}...},
    //           handle{std::forward<Handle>(handle)}
    //     {
    //     }

    //     void exec(const std::string &arg) = 0;
    // };
    template <typename Handle, typename... Policies_t>
    class Cmd_handle : public Base_cmd_handle
    {
        Handle handle;

    public:
        Cmd_handle(Handle &&handle)
            : Base_cmd_handle(std::move(Policy_ptr(new Policies_t{}))...),
              handle{std::forward<Handle>(handle)}
        {
        }

        void exec(const Data &arg) override
        {
            handle(arg);
        }
    };

    template <typename Handle, typename... Policies_t>
    Cmd_handle(Handle &&) -> Cmd_handle<Handle, Policies_t...>;

    template <typename... Policies_t>
    class Policies
    {
    public:
        template <typename Handle>
        static decltype(auto) make_cmd_handle(Handle &&handle)
        {
            return Cmd_handle<Handle, Policies_t ...>(std::forward<Handle>(handle));
        }

        template <typename Handle>
        class Cmd_handle : public Base_cmd_handle
        {
            Handle handle;
            
            template <typename... P>
            Policy_cont make_policy_cont()
            {
                Policy_cont cont;
                cont.emplace_back(Policy_ptr(new P{})...);
                return cont;
            }

        public:
            Cmd_handle(Handle &&handle)
                : Base_cmd_handle{std::move(make_policy_cont<Policies_t...>())},
                  handle{std::forward<Handle>(handle)}
            {
            }

            void exec(const std::vector<unsigned char> &arg)
            {
                // handle(arg);
            }
        };

        template <typename Handle>
        Cmd_handle(Handle &&) -> Cmd_handle<Handle>;
    };

    // template <typename Scallb, typename Rcallb>
    // Basic_controller(Scallb &&, Rcallb &&) -> Basic_controller<Scallb, Rcallb>;

    // template <typename Handle, typename... Policies>
    // Cmd_handle_pol(Handle handle) -> Cmd_handle_pol<Handle, Policies...>;

    // template <typename... Policies>
    // class P_cont
    // {
    // };

    // template <template <typename... Policies> typename T, typename Handle>
    // decltype(auto) make_cmd_handle(Handle &&handle)
    // {
    //     return Cmd_handle_pol<Handle, Policies...>(std::move(handle));
    // }

    class Ctrl_console final
    {
        using Ctrl_cmd_name = std::string;
        using Ctrl_cmd_handle = std::unique_ptr<Base_cmd_handle>;
        using Ctrl_cmd_pair = std::pair<Ctrl_cmd_name, Ctrl_cmd_handle>;
        using Cmds_cont = std::unordered_map<Ctrl_cmd_pair::first_type,
                                             Ctrl_cmd_pair::second_type>;
        using Data = std::vector<unsigned char>;

        Cmds_cont cmds;

        std::vector<std::string::iterator> find_words(Data::iterator begin, Data::iterator end)
        {

        }

    public:
        void exec(const Data &arg)
        {

        }

        template <typename Str, typename Cmd_handle_t>
        void add_cmd(Str &&cmd_name, Cmd_handle_t &&cmd_handle)
        {
            if (cmds.find(cmd_name) == cmds.end())
            {
                cmds.insert(Ctrl_cmd_pair{std::forward<Str>(cmd_name),
                                          Ctrl_cmd_handle(new Cmd_handle_t{std::forward<Cmd_handle_t>(cmd_handle)})});
            }
            else
            {
                throw std::logic_error{"The comand with specyfied name already exists!"};
            }
        }
    };
}