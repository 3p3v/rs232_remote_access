#pragma once

#include <memory>

#include <Notyfier.hpp>
#include <Timer_cont.hpp>

#include <Proto_rec.hpp>
#include <Remote_rec.hpp>
#include <Serial_rec.hpp>

namespace Logic
{

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

        // std::unique_ptr<Base_lock_factory> lock_factory;

    public:
        /// @brief Timer manager
        Timer_cont timer_man;

        /// @brief User notification
        Notifier notifier;

        /// @brief Device record
        Proto_rec rec;

        /// @brief Information needed to connect to MQTT topics
        Remote_rec &remote_rec; // TODO remove

        /// @brief Information needed to connect to serial port
        Serial_rec &serial_rec; // TODO remove

        /// @brief Communication with remote
        Remote_impl_t remote;

        /// @brief Communication with local serial
        Serial_impl_t serial;

        // /// @brief Lock mutex
        // /// @return
        // std::unique_ptr<Base_lock> make_lock() noexcept;

        /// @brief Create new timer
        /// @return Timer
        template <typename... Args_t>
        auto make_timer(Args_t &...args) noexcept;

        /// @brief
        /// @tparam Proto_helpers_ptr_t
        /// @tparam Remote_impl_t
        /// @tparam Serial_impl_t
        /// @param notifier
        /// @param dev
        /// @param rec
        /// @param remote
        /// @param serial
        template <
            typename Remote_impl_t,
            typename Serial_impl_t>
        Proto_helpers(
            // Base_lock_factory &&lock_factory,
            Notifier &&notifier,
            Proto_rec &&rec,
            Remote_rec &remote_rec,
            Serial_rec &serial_rec,
            Remote_impl_t &&remote,
            Serial_impl_t &&serial);

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
        typename Remote_impl_t,
        typename Serial_impl_t>
    inline Proto_helpers<
        Timer_factory_t,
        Remote_impl_t,
        Serial_impl_t>::Proto_helpers(
        // Base_lock_factory &&lock_factory,
        Notifier &&notifier_,
        Proto_rec &&rec,
        Remote_rec &remote_rec,
        Serial_rec &serial_rec,
        Remote_impl_t &&remote_,
        Serial_impl_t &&serial_)
        : // lock_factory{std::make_unique<Base_lock_factory>(std::move(lock_factory))},
          notifier{std::move(notifier)},
          rec{std::move(rec)},
          remote_rec{remote_rec},
          serial_rec{serial_rec},
          remote{std::forward<Remote_impl_t>(remote_)},
          serial{std::forward<Serial_impl_t>(serial_)}
    {
    }

    // template <typename Timer_factory_t, typename Remote_impl_t, typename Serial_impl_t>
    // inline std::unique_ptr<Base_lock> Proto_helpers<Timer_factory_t, Remote_impl_t, Serial_impl_t>::make_lock() noexcept
    // {
    //     return lock_factory.create();
    // }

    template <typename Timer_factory_t, typename Remote_impl_t, typename Serial_impl_t>
    template <typename... Args_t>
    inline auto
    Proto_helpers<Timer_factory_t, Remote_impl_t, Serial_impl_t>::make_timer(Args_t &...args) noexcept
    {
        return Timer_factory_t::make_timer(std::forward<Args_t>(args), ...);
    }

}