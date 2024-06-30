#pragma once

#include <unordered_map>
#include <algorithm>

namespace Mqtt_port
{
    template <typename Option, typename Val_type>
    class Universal_opt
    {
    private:
        using Opt_cont_type = std::unordered_map<Option, Val_type>;

        mutable Opt_cont_type options{};

        void check_if_exists(Option opt);

    public:
        bool emplace(Option option, Val_type &&val);
        bool emplace(Option option, const Val_type &val);

        template <typename Func>
        void for_each(Func &func) const noexcept;

        Val_type get(Option opt) const;

        size_t size() const noexcept;

        Universal_opt();
        Universal_opt(std::initializer_list<std::pair<const Option, Val_type>> init);
    };

    template <typename Option, typename Val_type>
    inline void Universal_opt<Option, Val_type>::check_if_exists(Option opt)
    {
        if (options.find(opt) != options.end())
            throw std::logic_error{"Element already exists!"};
    }

    template <typename Option, typename Val_type>
    bool Universal_opt<Option, Val_type>::emplace(Option option, Val_type &&val)
    {
        check_if_exists(option);
        auto ret = options.emplace(option, std::move(val));
        return ret.second;
    }

    template <typename Option, typename Val_type>
    bool Universal_opt<Option, Val_type>::emplace(Option option, const Val_type &val)
    {
        check_if_exists(option);
        auto ret = options.emplace(option, val);
        return ret.second;
    }

    template <typename Option, typename Val_type>
    template <typename Func>
    void Universal_opt<Option, Val_type>::for_each(Func &func) const noexcept
    {
        std::for_each(
            options.begin(), options.end(),
            [&func](auto &opt)
            {
                func(opt);
            });

        options.clear();
    }

    template <typename Option, typename Val_type>
    Val_type Universal_opt<Option, Val_type>::get(Option opt) const
    {
        Val_type ret;

        auto itr = options.find(opt);
        if (itr == options.end())
            throw std::logic_error{"Element does not exist!"};

        ret = std::move(itr->second);

        options.erase(itr);

        return ret;
    }

    template <typename Option, typename Val_type>
    inline size_t Universal_opt<Option, Val_type>::size() const noexcept
    {
        return options.size();
    }

    template <typename Option, typename Val_type>
    inline Universal_opt<Option, Val_type>::Universal_opt() = default;

    template <typename Option, typename Val_type>
    inline Universal_opt<Option, Val_type>::Universal_opt(std::initializer_list<std::pair<const Option, Val_type>> init)
        : options(init)
    {
    }
}