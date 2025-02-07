#pragma once

#include <Basic_timer.hpp>

namespace Logic
{
    template <typename Callb>
    class Timer_dummy : public Basic_timer
    {
        /* Callback */
        Callb callb;

        static Basic_timer* active;

    public:
        static void force_timeout()
        {
            active->callb();
        }

        void start() override
        {
        }

        void stop() override
        {
        }

        Timer_dummy(Callb &&callb, std::chrono::milliseconds interval = std::chrono::seconds{10})
            : Basic_timer{interval}, callb{std::move(callb)}
        {
            active = this;
        }

        ~Timer_dummy()
        {
            if (active == this)
                active = nullptr;
        }
    };

    template <typename Callb>
    Basic_timer *Timer_dummy<Callb>::active{nullptr};

    class Timer_dummy_maker
    {
    public:
        template <typename Callb>
        static auto make_timer(Callb &&callb, std::chrono::milliseconds interval = std::chrono::seconds{10})
        {
            return std::make_unique<Timer_dummy<std::decay_t<Callb>>>(std::forward<Callb>(callb), interval);
        }
    };
}