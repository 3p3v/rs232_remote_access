#pragma once

#include <memory>

#include <Timer_cont.hpp>
#include <Base_lock.hpp>

namespace Logic
{
    template <typename Handle_t>
    class Base_callback_wrapper
    {
    public:
        Base_callback_wrapper(Handle_t &&handle);

        template <typename... Args_t>
        decltype(auto) operator()(Args_t &&...args);
    };

    template <typename Handle_t>
    template <typename... Args_t>
    inline decltype(auto) Base_callback_wrapper<Handle_t>::operator()(Args_t &&...args)
    {

    }
    
    /// @brief Representation of class contains all extensions and other objects
    ///        needed to communicate between serial and remote device in respect to protocol
    /// @tparam Timer_factory_t Timer implementation
    /// @tparam Remote_impl_t Remote implementation
    /// @tparam Serial_impl_t Serial implementation
    template <
        typename Timer_factory_t, // TODO save as object
        typename Remote_impl_t,
        typename Serial_impl_t>
    class Proto_helpers
    {
        using Proto_helpers_type = Proto_helpers<
            Timer_factory_t,
            Remote_impl_t,
            Serial_impl_t>;

    public:
        /// @brief Timer manager
        Timer_cont timer_man;

        std::unique_ptr<Base_lock_factory> lock_factory;

        /// @brief User notification
        Notifier notifier;

        /// @brief Proto_helpers record
        Remote_dev rec;

        /// @brief Communication with remote
        Remote_impl_t remote;

        /// @brief Communication with local serial
        Serial_impl_t serial;

        /// @brief Lock mutex
        /// @return 
        std::unique_ptr<Base_lock> make_lock() noexcept;

        /// @brief Create new timer
        /// @return Timer
        template <typename... Args_t>
        auto make_timer(Args_t &...args) noexcept;

        /// @brief
        /// @tparam Proto_helpers_ptr_t
        /// @tparam Remote_impl_ptr_t
        /// @tparam Serial_impl_ptr_t
        /// @param notifier
        /// @param dev
        /// @param rec
        /// @param remote
        /// @param serial
        template <
            typename Remote_impl_ptr_t,
            typename Serial_impl_ptr_t>
        Proto_helpers(
            Base_lock_factory &&lock_factory,
            Notifier &&notifier,
            Remote_dev &&rec,
            Remote_impl_ptr_t &&remote,
            Serial_impl_ptr_t &&serial);

        Proto_helpers(Proto_helpers &&) = default;
        Proto_helpers &operator=(Proto_helpers &&) = default;
        Proto_helpers(const Proto_helpers &) = delete;
        Proto_helpers &operator=(const Proto_helpers &) = delete;
        virtual ~Proto_helpers() = default;
    };

    template <
        typename Timer_factory_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    template <
        typename Remote_impl_ptr_t,
        typename Serial_impl_ptr_t>
    inline Proto_helpers<
        Timer_factory_t,
        Remote_impl_t,
        Serial_impl_t>::Proto_helpers(Base_lock_factory &&lock_factory,
                                      Notifier &&notifier_,
                                      Remote_dev &&rec__,
                                      Remote_impl_ptr_t &&remote_,
                                      Serial_impl_ptr_t &&serial_)
        : lock_factory{std::make_unique<Base_lock_factory>(std::move(lock_factory))},
          notifier{std::move(notifier)},
          rec_(std::move(rec)),
          remote{std::forward<Remote_impl_ptr_t>(remote_)},
          serial{std::forward<Serial_impl_ptr_t>(serial_)}
    {
    }

    template <typename Timer_factory_t, typename Remote_impl_t, typename Serial_impl_t>
    inline std::unique_ptr<Base_lock> Proto_helpers<Timer_factory_t, Remote_impl_t, Serial_impl_t>::make_lock() noexcept
    {
        return lock_factory.create();
    }

    template <typename Timer_factory_t, typename Remote_impl_t, typename Serial_impl_t>
    template <typename... Args_t>
    inline auto
    Proto_helpers<Timer_factory_t, Remote_impl_t, Serial_impl_t>::make_timer(Args_t &...args) noexcept
    {
        return Timer_factory_t::make_timer(std::forward<Args_t>(args), ...);
    }
    
}