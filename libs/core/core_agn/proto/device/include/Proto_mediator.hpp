#pragma once

#include <memory>

#include <Base_proto_mediator.hpp>
#include <Proto_helpers.hpp>
#include <Proto_module.hpp>
#include <Greeter.hpp>
#include <Ctrl_console.hpp>
#include <Remote_rec.hpp>
#include <Serial_rec.hpp>

namespace Logic
{
    template <typename Helpers_t, typename Wrapper_t>
    class Proto_mediator final
        : public Base_proto_mediator,
          public std::enable_shared_from_this<Proto_mediator<Helpers_t, Wrapper_t>>
    {
    protected:
        void integrity_check();

        /// @brief Greeting module (for starting communication)
        std::unique_ptr<Base_greeter> greeter{nullptr};

        /// @brief Exchanger module
        std::unique_ptr<Base_exchanger> exchanger{nullptr};

    public:
        /// @brief Container for all commands
        Cmd_ctrl::Ctrl_console cmds;

        Wrapper_t wrapper_factory;

        /// @brief Information needed to connect to MQTT topics
        Remote_rec remote_rec;

        /// @brief Information needed to connect to serial port
        Serial_rec serial_rec;

        /// @brief Common objects
        Helpers_t helpers; // TODO delete

        void start() override final;
        void restart() override final;

        Proto_rec& get_rec() const noexcept override;
        Proto_rec& get_rec() const noexcept override;
        Serial_rec& get_serial_rec() const noexcept override;

        std::shared_ptr<Base_proto_mediator> get_shared();
        std::weak_ptr<Base_proto_mediator> get_weak();

        template <typename Handle_t>
        auto make_weak(Handle_t &&handle);
        template <typename Handle_t>
        auto make_shared(Handle_t &&handle);

        void set_greeter(std::unique_ptr<Base_greeter> &&greeter);
        void set_exchanger(std::unique_ptr<Base_exchanger> &&exchanger);

        template <
            typename Remote_impl_factory_t,
            typename Serial_impl_factory_t>
        Proto_mediator(
            Wrapper_t &&wrapper,
            Notifier &&notifier,
            Proto_rec &&rec,
            Remote_rec &remote_rec,
            Serial_rec &serial_rec,
            Remote_impl_factory_t &&remote_factory,
            Serial_impl_factory_t &&serial_factory);
    };

    template <typename Helpers_t, typename Wrapper_t>
    inline std::shared_ptr<Base_proto_mediator>
    Proto_mediator<Helpers_t, Wrapper_t>::get_shared()
    {
        return shared_from_this();
    }

    template <typename Helpers_t, typename Wrapper_t>
    inline std::weak_ptr<Base_proto_mediator>
    Proto_mediator<Helpers_t, Wrapper_t>::get_weak()
    {
        return weak_from_this();
    }

    template <typename Helpers_t, typename Wrapper_t>
    inline void Proto_mediator<Helpers_t, Wrapper_t>::set_greeter(std::unique_ptr<Base_greeter> &&greeter)
    {
        greeter = greeter;
    }

    template <typename Helpers_t, typename Wrapper_t>
    inline void Proto_mediator<Helpers_t, Wrapper_t>::set_exchanger(std::unique_ptr<Base_exchanger> &&exchanger)
    {
        exchanger = exchanger;
    }

    template <typename Helpers_t, typename Wrapper_t>
    inline void Proto_mediator<Helpers_t, Wrapper_t>::integrity_check()
    {
        if (!(*greeter || *exchanger))
        {
            throw std::logic_error{"Protocol not initialized."}
        }
    }

    template <typename Helpers_t, typename Wrapper_t>
    inline void Proto_mediator<Helpers_t, Wrapper_t>::start()
    {
        integrity_check();

        /* Connect to info topic */
        remote.connect_i(
            /* Callback used when receiving message */
            make_weak(
                [this](auto begin, auto end, auto callb)
                {
                    if (auto p = ptr.lock())
                    {
                        auto lock = helpers.make_lock();

                        /* Forward message to interpreter */
                        manager.cmd_exec.exec(begin, end);

                        /* Run callback releasing resources etc. */
                        callb();
                    }
                }),
            /* Callback used when could not collect or had exception while reading */
            make_weak(
                [this](const auto &e)
                {
                    if (auto p = ptr.lock())
                    {
                        auto lock = helpers.make_lock();

                        // TODO send error to monitor
                    }
                }),
            /* Subscribe success */
            make_weak(
                [this]()
                {
                    if (auto p = ptr.lock())
                    {
                        auto lock = helpers.make_lock();

                        /* Try to greet the remote */
                        greeter->greet();
                    }
                }),
            /* Subscribe error */
            make_weak(
                [this](const auto &e)
                {
                    if (auto p = ptr.lock())
                    {
                        auto lock = helpers.make_lock();

                        // TODO send error to monitor
                    }
                }));

        /* Connect to data channels */
        exchanger->start();
    }

    template <typename Helpers_t, typename Wrapper_t>
    inline void Proto_mediator<Helpers_t, Wrapper_t>::restart()
    {
        integrity_check();

        timer_man.clear();
        greeter->restart();
        exchanger->restart();
        helpers.rec.status = Remote_status::Disconnected;
        /* Try to greet the remote */
        greeter->greet();
    }

    
    template <typename Helpers_t, typename Wrapper_r>
    inline Proto_rec& Proto_mediator<Helpers_t, Wrapper_t>::get_rec() const noexcept
    {
        return helpers.rec;
    }
    
    template <typename Helpers_t, typename Wrapper_r>
    inline Remote_rec& Proto_mediator<Helpers_t, Wrapper_t>::get_remote_rec() const noexcept
    {
        return remote_rec;
    }
    
    template <typename Helpers_t, typename Wrapper_r>
    inline Serial_rec& Proto_mediator<Helpers_t, Wrapper_t>::get_serial_rec() const noexcept
    {
        return serial_rec;
    }

    template <typename Helpers_t, typename Wrapper_t>
    template <typename Handle_t>
    inline auto Proto_mediator<Helpers_t, Wrapper_t>::make_weak(Handle_t &&handle)
    {
        return wrapper_factory.make_weak(make_weak_from_this(), std::forward<Handle_t>(handle));
    }

    template <typename Helpers_t, typename Wrapper_t>
    template <typename Handle_t>
    inline auto Proto_mediator<Helpers_t, Wrapper_t>::make_shared(Handle_t &&handle)
    {
        return wrapper_factory.make_shared(make_shared_from_this(), std::forward<Handle_t>(handle));
    }

    template <typename Helpers_t, typename Wrapper_t>
    template <typename Remote_impl_factory_t, typename Serial_impl_factory_t>
    inline Proto_mediator<Helpers_t, Wrapper_t>::Proto_mediator(
        Wrapper_t &&wrapper,
        Notifier &&notifier,
        Proto_rec &&rec,
        Remote_rec &remote_rec,
        Serial_rec &serial_rec,
        Remote_impl_factory_t &&remote_factory,
        Serial_impl_factory_t &&serial_factory)
        : wrapper{std::move(wrapper)},
          remote_rec{std::move(remote_rec)},
          serial_rec{std::move(serial_rec)},
          helpers{
              std::move(notifier),
              std::move(rec),
              this->remote_rec,
              this->serial_rec,
              std::move(remote_factory.create(this->remote_rec)),
              std::move(serial_factory.create(this->serial_rec))}

    {
    }
}