#pragma once

#include <memory>

#include <Device_prereq.hpp>
#include <Timer_cont.hpp>

namespace Logic
{
    /// @brief Representation of class contains all extensions and other objects
    ///        needed to communicate between serial and remote device in respect to protocol
    /// @tparam Timer_factory_t Timer implementation
    /// @tparam Remote_impl_t Remote implementation
    /// @tparam Serial_impl_t Serial implementation
    template <
        typename Timer_factory_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    class Device : public Device_prereq, std::enable_shared_from_this
    {
        using Device_type = Device<
            Timer_factory_t,
            Remote_impl_t,
            Serial_impl_t>;

    public:
        Remote_impl_t remote;
        Serial_impl_t serial;

        void start();
        void restart();

        std::shared_ptr<Device_prereq> get_shared();
        std::weak_ptr<Device_prereq> get_weak();

        Timer_factory_t make_timer() noexcept;

        /// @brief 
        /// @tparam Device_ptr_t 
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
        Device(
            Notifier &&notifier,
            Remote_dev &&rec,
            std::unique_ptr<Proto_module>&& exchanger,
            Remote_impl_ptr_t &&remote,
            Serial_impl_ptr_t &&serial);

        Device(Device &&) = default;
        Device &operator=(Device &&) = default;
        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;
        virtual ~Device() = default;
    };

    template <
        typename Timer_factory_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    inline std::shared_ptr<Device_prereq> 
    Device<
        Timer_factory_t,
        Remote_impl_t,
        Serial_impl_t>::get_shared()
    {
        return shared_from_this();
    }

    template <
        typename Timer_factory_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    inline std::weak_ptr<Device_prereq> 
    Device<
        Timer_factory_t,
        Remote_impl_t,
        Serial_impl_t>::get_weak()
    {
        return weak_from_this();
    }

    template <
        typename Timer_factory_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    inline auto 
    Device<
        Timer_factory_t,
        Remote_impl_t,
        Serial_impl_t>::make_timer() noexcept
    {
        return Timer_factory_t::make_timer();
    }

    template <
        typename Timer_factory_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    inline Device<
        Timer_factory_t,
        Remote_impl_t,
        Serial_impl_t>::start()
    {
        /* Connect to settings topic */
        remote.connect(
            /* Callback used when receiving message */
            [this, ptr = dev](auto begin, auto end, auto callb)
            {
                if (auto p = ptr.lock())
                {
                    /* Forward message to interpreter */
                    manager.cmd_exec.exec(begin, end);

                    /* Run callback releasing resources etc. */
                    callb();
                }
            },
            /* Callback used when could not collect or had exception while reading */
            [](const auto &e)
            {
                // TODO send error to monitor
            });

        /* Connect to data channels */
        // TODO
    }

    template <
        typename Timer_factory_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    inline Device<
        Timer_factory_t,
        Remote_impl_t,
        Serial_impl_t>::restart()
    {
        timer_man.clear();
        exchanger->restart();
        device.rec.status = Remote_status::Disconnected;
        /* Try to greet the remote */
        
    }

    template <
        typename Timer_factory_t,
        typename Remote_impl_t,
        typename Serial_impl_t>
    template <
        typename Remote_impl_ptr_t,
        typename Serial_impl_ptr_t>
    inline Device<
        Timer_factory_t,
        Remote_impl_t,
        Serial_impl_t>::Device(
            Notifier &&notifier_,
            Remote_dev &&rec__,
            std::unique_ptr<Proto_module>&& exchanger,
            Remote_impl_ptr_t &&remote_,
            Serial_impl_ptr_t &&serial_)
        : Device_prereq{std::move(notifier_), std::move(rec__), std::move(exchanger)},
          remote{std::forward<Remote_impl_ptr_t>(remote_)},
          serial{std::forward<Serial_impl_ptr_t>(serial_)}
    {
        
    }
}