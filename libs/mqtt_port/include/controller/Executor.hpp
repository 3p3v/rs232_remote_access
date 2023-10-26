#pragma once

#include <vector>
#include <memory>

namespace Mqtt_port
{   
    class Executor
    {
    public:
        // template <typename E>
        // using Ptr_t = std::shared_ptr<Executor>;
        using Data = std::vector<unsigned char>;

    public:

        virtual void exec(const std::string &ch_name, const Executor::Data::const_iterator begin, const Executor::Data::const_iterator end) = 0;
    };

    template <typename Handle>
    class Dyn_executor : public Executor
    {
        Handle handle;

    public:
        Dyn_executor(Handle &&handle)
            : handle{std::move(handle)}
        {
        }

        void exec(const std::string &ch_name, const Executor::Data::const_iterator begin, const Executor::Data::const_iterator end) override
        {
            handle(ch_name, begin, end);
        }

    };

    template <typename Handle>
    Dyn_executor(Handle &&) -> Dyn_executor<Handle>;

    // template <typename E, typename... Args>
    // decltype(auto) make_ptr(Args... args)
    // {
    //     return E::Ptr_t<E>(new E(args...));
    // }
}