#pragma once

#include <algorithm>
#include <stdexcept>

namespace Mqtt_port
{
    /// @brief Universal getter class used to store options and corresponding arguments
    /// @tparam Class_opt class containing definition of options in enum
    template <typename Option, typename Val_type>
    class Universal_get_opt
    {
    public:
        using Opt_pair = std::tuple<const Option, Val_type, bool>;
        using Opt_cont_type = std::vector<Opt_pair>;

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
                      [](Opt_pair &opt)
                      {
                          if (get<2>(opt) == true)
                              func(get<1>(opt));
                      });
    }

    template <typename Option, typename Val_type>
    std::string Universal_get_opt<Option, Val_type>::get(Option opt)
    {
        std::for_each(options.begin(), options.end(),
                      [](Opt_pair &option)
                      {
                          if (get<0>(option) == opt && get<2>(option) == true)
                          {
                              get<2>(option) return std::move(get<1>(option));
                          }
                          else if (get<2>(option) = false)
                          {
                              std::logic_error{"Parameter was already used!"};
                          }
                          else
                          {
                              std::logic_error{"Parameter not provided!"};
                          }
                      });
    }

}