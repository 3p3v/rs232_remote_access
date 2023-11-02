#pragma once

#include <algorithm>
#include <vector>
#include <stdexcept>

namespace Mqtt_port
{
    /// @brief Universal getter class used to store options and corresponding arguments
    /// @tparam Class_opt class containing definition of options in enum
    template <typename Option, typename Val_type>
    class Universal_get_opt
    {
    public:
        using Opt_tuple = std::tuple<const Option, Val_type, bool>;

    private:
        using Opt_cont_type = std::vector<Opt_tuple>;

    protected:
        Opt_cont_type options;

    public:
        template <typename Func>
        void for_each(Func &&func) noexcept;

        std::string get(Option opt);
    };

    template <typename Option, typename Val_type>
    template <typename Func>
    void Universal_get_opt<Option, Val_type>::for_each(Func &&func) noexcept
    {
        std::for_each(options.begin(), options.end(),
                      [&func](Opt_tuple &opt)
                      {
                          if (std::get<2>(opt) == true)
                              func(opt);
                      });
    }

    template <typename Option, typename Val_type>
    std::string Universal_get_opt<Option, Val_type>::get(Option opt)
    {
        std::string ret;
        
        std::for_each(options.begin(), options.end(),
                      [&opt, &ret](Opt_tuple &option)
                      {
                          if (std::get<0>(option) == opt && std::get<2>(option) == true)
                          {
                               std::get<2>(option) = false;
                               ret = std::move(std::get<1>(option));
                               return;
                          }
                          else if (std::get<2>(option) == false)
                          {
                              std::logic_error{"Parameter was already used!"};
                          }
                          else
                          {
                              std::logic_error{"Parameter not provided!"};
                          }
                      });

        return ret;
    }

}