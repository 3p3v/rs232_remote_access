#pragma once

#include <memory>
#include <mutex>

namespace Logic
{
    template <
        typename Device_t,
        typename Handle_t>
    class Locked_weak_callback_wrapper
    {
        Handle_t handle;
        std::weak_ptr<Device_t> ptr;
        std::mutex &mutex;

    public:
        Locked_weak_callback_wrapper(
            std::weak_ptr<Device_t> &&ptr,
            std::mutex &mutex,
            Handle_t &&handle);

        template <typename... Args_t>
        decltype(auto) operator()(Args_t &&...args);
    };

    template <
        typename Device_t,
        typename Handle_t>
    class Locked_shared_callback_wrapper
    {
        Handle_t handle;
        std::shared_ptr<Device_t> ptr;
        std::mutex &mutex;

    public:
        Locked_shared_callback_wrapper(
            std::shared_ptr<Device_t> &&ptr,
            std::mutex &mutex,
            Handle_t &&handle);

        template <typename... Args_t>
        decltype(auto) operator()(Args_t &&...args);
    };

    template <typename Device_t>
    class Locked_callback_wrapper
    {
    public:
        Locked_callback_wrapper(std::mutex &mutex);

        template <typename Handle_t>
        Locked_weak_callback_wrapper<Device_t, Handle_t> make_weak(std::weak_ptr<Device_t> &&ptr, Handle_t &&handle);

        template <typename Handle_t>
        Locked_shared_callback_wrapper<Device_t, Handle_t> make_shared(std::shared_ptr<Device_t> &&ptr, Handle_t &&handle);
    };

    template <typename Device_t, typename Handle_t>
    inline Locked_weak_callback_wrapper<Device_t, Handle_t>::Locked_weak_callback_wrapper(
        std::weak_ptr<Device_t> &&ptr, std::mutex &mutex, Handle_t &&handle)
        : ptr{std::move(ptr)}, mutex{mutex}, handle{std::move(handle)}
    {
    }

    template <typename Device_t, typename Handle_t>
    inline Locked_shared_callback_wrapper<Device_t, Handle_t>::Locked_shared_callback_wrapper(
        std::shared_ptr<Device_t> &&ptr, std::mutex &mutex, Handle_t &&handle)
        : ptr{std::move(ptr)}, mutex{mutex}, handle{std::move(handle)}
    {
    }

    template <typename Device_t, typename Handle_t>
    template <typename... Args_t>
    inline decltype(auto) Locked_weak_callback_wrapper<Device_t, Handle_t>::operator()(Args_t &&...args)
    {
        if (auto p = ptr.lock())
        {
            auto lock = std::scope_guard<std::mutex>{mutex};
            return handle(std::forward<Args_t>(args), ...);
        }
    }

    template <typename Device_t, typename Handle_t>
    template <typename... Args_t>
    inline decltype(auto) Locked_shared_callback_wrapper<Device_t, Handle_t>::operator()(Args_t &&...args)
    {
        auto lock = std::scope_guard<std::mutex>{mutex};
        return handle(std::forward<Args_t>(args), ...);
    }

    template <typename Device_t>
    template <typename Handle_t>
    inline Locked_weak_callback_wrapper<Device_t, Handle_t> Locked_callback_wrapper<Device_t>::make_weak(
        std::weak_ptr<Device_t> &&ptr, Handle_t &&handle)
    {
        return Locked_weak_callback_wrapper<Device_t, Handle_t>(std::move(ptr), mutex, std::move(handle));
    }

    template <typename Device_t>
    template <typename Handle_t>
    inline Locked_shared_callback_wrapper<Device_t, Handle_t> Locked_callback_wrapper<Device_t>::make_shared(
        std::shared_ptr<Device_t> &&ptr, Handle_t &&handle)
    {
        return Locked_shared_callback_wrapper<Device_t, Handle_t>(std::move(ptr), mutex, std::move(handle));
    }

}