#pragma once

#include <Universal_get_opt.hpp>

namespace Mqtt_port
{
    template <tyename Class_opt, typename Val_type>
    class Universal_opt : public Universal_get_opt<Class_opt, Val_type>
    {
        bool find(Option option);

    public:
        // bool push_option(const Opt_pair &option);
        // bool push_option(Opt_pair &&option);
        bool emplace(Option option, Val_type &&val);
        bool emplace(Option option, const Val_type &val);
    };
    
    template <typename Class_opt, typename Val_type>
    bool Universal_opt<Class_opt, Val_type>::find(Option option)
    {
        if (std::find(options.begin(), options.end(), option) == options.end())
            return false;
        else 
            return true;
    }

    template <typename Class_opt, typename Val_type>
    bool Universal_opt<Class_opt, Val_type>::emplace(Option option, Val_type &&val)
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
    bool Universal_opt<Class_opt, Val_type>::emplace(Option option, const Val_type &val)
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
    
}