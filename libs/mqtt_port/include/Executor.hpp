#pragma once

#include <vector>
#include <memory>

namespace Mqtt_port
{   
    class Executor
    {
    public:
        // template <typename E>
        // using Ptr_t = std::unique_ptr<E>;
        using Data = std::vector<unsigned char>;

    public:
        virtual void exec(const Data &data) {};
    };

    // template <typename E, typename... Args>
    // decltype(auto) make_ptr(Args... args)
    // {
    //     return E::Ptr_t<E>(new E(args...));
    // }
}