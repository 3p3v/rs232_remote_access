#pragma once

#include <Remote_record.hpp>

namespace Logic
{
    template <typename Mqtt, typename Serial>
    class Remote_unlock : public Remote_record<Mqtt, Serial>
    {
    public:
        void lock();
        void unlock();
    };
    
    template <typename Mqtt, typename Serial>
    inline void Remote_unlock<Mqtt, Serial>::lock()
    {
        lock_ = true;
    }

    template <typename Mqtt, typename Serial>
    inline void Remote_unlock<Mqtt, Serial>::unlock()
    {
        lock_ = false;
    }
}