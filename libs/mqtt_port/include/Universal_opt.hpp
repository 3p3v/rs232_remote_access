#pragma once

#include <Universal_get_opt.hpp>
#include <string>

namespace Mqtt_port
{
    template <typename Option, typename Val_type = std::string>
    class Universal_opt : public Universal_get_opt<Option, Val_type>
    {
        void find(Option option);

    public:
        void emplace(Option option, Val_type &&val);
        void emplace(Option option, const Val_type &val);
    };

    template <typename Option, typename Val_type>
    void Universal_opt<Option, Val_type>::find(Option option)
    {
        std::for_each(options.begin(), options.end(), [&option](auto &opt)
                      {
                        if (std::get<0>(opt) == option)
                        {
                            throw std::logic_error{"Seme parameter was used twice!"};
                        } });
    }

    template <typename Option, typename Val_type>
    void Universal_opt<Option, Val_type>::emplace(Option option, Val_type &&val)
    {
        find(option);
        options.emplace_back(option, std::move(val), true);
    }

    template <typename Option, typename Val_type>
    void Universal_opt<Option, Val_type>::emplace(Option option, const Val_type &val)
    {
        find(option);
        options.emplace_back(option, val, true);
    }

}