#pragma once

#include <Dev_ext.hpp>
#include <Remote_dev.hpp>
#include <Notyfier.hpp>

namespace Logic
{
    class Dev_adder : public Dev_ext
    {
        Worker_storage &notyfier;

    public:
        template <typename Dev_t, typename... Args_t>
        void add(Remote_dev &&rec, Args_t &&...args);

        Dev_adder(Dev_cont &devs, Worker_storage& notyfier);
    };
    
    template <typename Dev_t, typename... Args_t>
    inline void Dev_adder::add(Remote_dev &&rec, Args_t &&...args)
    {
        std::lock_guard<std::mutex> lock{devs.dev_mutex};        
        
        devs.devs.emplace(
            devs.num, 
            Device_holder::make_device<Dev_t>(
                Notyfier{devs.num, notyfier}, 
                std::move(rec), 
                std::forward<Args_t>(args)...));

        if (++devs.num == std::numeric_limits<Dev_num>::min())
            throw std::logic_error{"Too many devices connected. Limit reached."};
    }
} // namespace Logic
