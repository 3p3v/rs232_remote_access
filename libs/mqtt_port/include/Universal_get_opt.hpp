#pragma once

namespace Mqtt_port
{
    /// @brief Universal getter class used to store options and corresponding arguments
    /// @tparam Class_opt class containing definition of options in enum
    template <typename Class_opt, typename Val_type>
    class Universal_get_opt : public Class_opt
    {
    public:
        using Option = typename Class_opt::Option;
        using Opt_pair = std::pair<const Option, Val_type>;
        using Opt_cont_type = std::vector<Opt_pair>;

    private:
        Opt_cont_type options;
        bool used{false};

        bool find(Option option);

    protected:
        bool push_option(const Opt_pair &option);
        bool push_option(Opt_pair &&option);
        bool emplace_option(Option option, Val_type &&val);
        bool emplace_option(Option option, const Val_type &val);

    public:
        Opt_cont_type::iterator begin();

        Opt_cont_type::iterator end();
    };

    template <typename Class_opt, typename Val_type>
    bool Universal_get_opt<Class_opt, Val_type>::find(Option option)
    {
        if (std::find(options.begin(), options.end(), option) == options.end())
            return false;
        else 
            return true;
    }
    
    template <typename Class_opt, typename Val_type>
    bool Universal_get_opt<Class_opt, Val_type>::push_option(const Opt_pair &option)
    {
        if (auto found = find(option.first))
        {
            return !found;
        }
        else
        {
            options.push_back(option);
            return !found;
        }
    }

    template <typename Class_opt, typename Val_type>
    bool Universal_get_opt<Class_opt, Val_type>::push_option(Opt_pair &&option)
    {
        if (auto found = find(option.first))
        {
            return !found;
        }
        else
        {
            options.push_back(std::move(option));
            return !found;
        }
    }

    template <typename Class_opt, typename Val_type>
    bool Universal_get_opt<Class_opt, Val_type>::emplace_option(Option option, Val_type &&val)
    {
        if (auto found = find(option))
        {
            return !found;
        }
        else
        {
            options.emplace_back(option, std::move(val));
            return !found;
        }
    }

    template <typename Class_opt, typename Val_type>
    bool Universal_get_opt<Class_opt, Val_type>::emplace_option(Option option, const Val_type &val)
    {
        if (auto found = find(option))
        {
            return !found;
        }
        else
        {
            options.emplace_back(option, val);
            return !found;
        }
    }

    template <typename Class_opt, typename Val_type>
    Universal_get_opt<Class_opt, Val_type>::Opt_cont_type::iterator Universal_get_opt<Class_opt, Val_type>::begin()
    {
        if (!used)
        {
            used = true
            return options.begin();
        }
        else
        {
            return options.end();
        }
    }

    template <typename Class_opt, typename Val_type>
    Universal_get_opt<Class_opt, Val_type>::Opt_cont_type::iterator Universal_get_opt<Class_opt, Val_type>::end()
    {
        return options.end();
    }
    
}