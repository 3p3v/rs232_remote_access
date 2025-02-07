#pragma once

#include <memory>
#include <mutex>

namespace Logic
{
    template <
        typename Device_t,
        typename Handle_t>
    class Base_weak_callback_wrapper
    {
        Handle_t handle;
        std::weak_ptr<Device_t> ptr;

    public:
        Base_weak_callback_wrapper(
            std::weak_ptr<Device_t> &&ptr,
            Handle_t &&handle);

        template <typename... Args_t>
        decltype(auto) operator()(Args_t &&...args);
    };

    template <
        typename Device_t,
        typename Handle_t>
    class Base_shared_callback_wrapper
    {
        Handle_t handle;

    public:
        Base_shared_callback_wrapper(
            std::shared_ptr<Device_t> &&ptr,
            Handle_t &&handle);

        template <typename... Args_t>
        decltype(auto) operator()(Args_t &&...args);
    };

    template <typename Device_t>
    class Base_callback_wrapper
    {
    public:
        template <typename Handle_t>
        Base_weak_callback_wrapper<Device_t, Handle_t> make_weak(std::weak_ptr<Device_t> &&ptr, Handle_t &&handle);

        template <typename Handle_t>
        Base_shared_callback_wrapper<Device_t, Handle_t> make_shared(std::shared_ptr<Device_t> &&ptr, Handle_t &&handle);
    };

    template <typename Device_t, typename Handle_t>
    inline Base_weak_callback_wrapper<Device_t, Handle_t>::Base_weak_callback_wrapper(
        std::weak_ptr<Device_t> &&ptr, Handle_t &&handle)
        : ptr{std::move(ptr)}, handle{std::move(handle)}
    {
    }

    template <typename Device_t, typename Handle_t>
    inline Base_shared_callback_wrapper<Device_t, Handle_t>::Base_shared_callback_wrapper(
        std::shared_ptr<Device_t> &&ptr, Handle_t &&handle)
        : ptr{std::move(ptr)}, handle{std::move(handle)}
    {
    }

    template <typename Device_t, typename Handle_t>
    template <typename... Args_t>
    inline decltype(auto) Base_weak_callback_wrapper<Device_t, Handle_t>::operator()(Args_t &&...args)
    {
        if (auto p = ptr.lock())
        {
            return handle(std::forward<Args_t>(args), ...);
        }
    }

    template <typename Device_t, typename Handle_t>
    template <typename... Args_t>
    inline decltype(auto) Base_shared_callback_wrapper<Device_t, Handle_t>::operator()(Args_t &&...args)
    {
        return handle(std::forward<Args_t>(args), ...);
    }

    template <typename Device_t>
    template <typename Handle_t>
    inline Base_weak_callback_wrapper<Device_t, Handle_t> Base_callback_wrapper<Device_t>::make_weak(
        std::weak_ptr<Device_t> &&ptr, Handle_t &&handle)
    {
        return Base_weak_callback_wrapper<Device_t, Handle_t>(std::move(ptr), std::move(handle));
    }

    template <typename Device_t>
    template <typename Handle_t>
    inline Base_shared_callback_wrapper<Device_t, Handle_t> Base_callback_wrapper<Device_t>::make_shared(
        std::shared_ptr<Device_t> &&ptr, Handle_t &&handle)
    {
        return Base_shared_callback_wrapper<Device_t, Handle_t>(std::move(ptr), std::move(handle));
    }

}