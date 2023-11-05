#pragma once

#include <Executor.hpp>

namespace Mqtt_port
{
    template <typename Handle>
    class Dyn_executor final : public Executor
    {
        Handle handle;

    public:
        Dyn_executor(Handle &&handle);

        void exec(const std::string &ch_name, const Executor::Data::const_iterator begin, const Executor::Data::const_iterator end) override;
    };

    template <typename Handle>
    Dyn_executor<Handle>::Dyn_executor(Handle &&handle)
        : handle{std::forward<Handle>(handle)}
    {
    }

    template <typename Handle>
    void Dyn_executor<Handle>::exec(const std::string &ch_name, const Executor::Data::const_iterator begin, const Executor::Data::const_iterator end)
    {
        handle(ch_name, begin, end);
    }

    template <typename Handle>
    Dyn_executor(Handle &&) -> Dyn_executor<Handle>;
}