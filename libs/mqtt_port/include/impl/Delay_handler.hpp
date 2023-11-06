#pragma once

#include <vector>
#include <algorithm>

namespace Mqtt_port
{
    namespace Impl
    {
        /// @brief
        /// @tparam Delayed_t
        template <typename Delayed_t>
        class Delay_handler final
        {
            std::vector<Delayed_t> d_cont;

        public:
            template <typename... Args>
            void emplace(Args &&...args);

            Delayed_t pop();

            bool empty();

            template <typename Func>
            void for_each(Func &&func);
        };

        template <typename Delayed_t>
        template <typename... Args>
        void Delay_handler<Delayed_t>::emplace(Args &&...args)
        {
            d_cont.emplace_back(std::forward<Args>(args)...);
        }

        template <typename Delayed_t>
        Delayed_t Delay_handler<Delayed_t>::pop()
        {
            auto d_obj = std::move(d_cont.back());
            d_cont.pop_back();
            return std::move(d_obj);
        }

        template <typename Delayed_t>
        bool Delay_handler<Delayed_t>::empty()
        {
            return d_cont.empty();
        }

        template <typename Delayed_t>
        template <typename Func>
        void Delay_handler<Delayed_t>::for_each(Func &&func)
        {
            std::for_each(d_cont.begin(), d_cont.end(), 
                          [&func](auto &d)
                          {
                            func(std::move(d));
                          });

            d_cont.clear();
        }
    }
}