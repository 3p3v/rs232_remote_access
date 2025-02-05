#pragma once

#include <memory>

#include <Device.hpp>
#include <Proto_module.hpp>
#include <Greeter.hpp>
#include <Ctrl_console.hpp>

namespace Logic
{
    class Base_proto_mediator
    {
    public:
        virtual void start() = 0;

        Base_proto_mediator() = default;
        Base_proto_mediator(Base_proto_mediator &&) = delete;
        Base_proto_mediator(const Base_proto_mediator &) = delete;
        Base_proto_mediator &operator=(Base_proto_mediator &&) = delete;
        Base_proto_mediator &operator=(const Base_proto_mediator &) = delete;
        virtual~Base_proto_mediator() = default;
    };

    template <typename Device_t>
    class Proto_mediator final
        : public Base_proto_mediator,
          public std::enable_shared_from_this<Proto_mediator<Device_t>>
    {
    protected:
        void integrity_check();

        /// @brief Greeting module (for starting communication)
        std::unique_ptr<Base_greeter> greeter{nullptr};

        /// @brief Exchanger module
        std::unique_ptr<Base_exchager> exchanger{nullptr};

        /// @brief Container for all commands
        Cmd_ctrl::Ctrl_console cmds;

    public:
        /// @brief Common objects
        Device_t device;

        void start() override final;
        void restart();

        std::shared_ptr<Base_proto_mediator> get_shared();
        std::weak_ptr<Base_proto_mediator> get_weak();

        void set_greeter(std::unique_ptr<Base_greeter> &&greeter);
        void set_exchanger(std::unique_ptr<Base_exchanger> &&exchanger);
    };

    template <typename Device_t>
    inline std::shared_ptr<Base_proto_mediator>
    Proto_mediator<Device_t>::get_shared()
    {
        return shared_from_this();
    }

    template <typename Device_t>
    inline std::weak_ptr<Base_proto_mediator>
    Proto_mediator<Device_t>::get_weak()
    {
        return weak_from_this();
    }

    template <typename Device_t>
    inline void Proto_mediator<Device_t>::set_greeter(std::unique_ptr<Base_greeter> &&greeter)
    {
        greeter = greeter;
    }

    template <typename Device_t>
    inline void Proto_mediator<Device_t>::set_exchanger(std::unique_ptr<Base_exchanger> &&exchanger)
    {
        exchanger = exchanger;
    }

    template <typename Device_t>
    inline void Proto_mediator<Device_t>::integrity_check()
    {
        if (!(*greeter || *exchanger))
        {
            throw std::logic_error{"Protocol not initialized."}
        }
    }

    template <typename Device_t>
    inline void Proto_mediator<Device_t>::start()
    {
        integrity_check();
        
        /* Connect to info topic */
        remote.connect_i(
            /* Callback used when receiving message */
            [this, ptr = weak_from_this()](auto begin, auto end, auto callb)
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
            },
            /* Subscribe success */
            [this, ptr = weak_from_this()]()
            {
                if (auto p = ptr.lock())
                {
                    /* Try to greet the remote */
                    greeter->greet();
                }
            },
            /* Subscribe error */
            [](const auto &e)
            {
                // TODO send error to monitor
            },);

        /* Connect to data channels */
        exchanger->start();
    }

    template <typename Device_t>
    inline void Proto_mediator<Device_t>::restart()
    {
        integrity_check();
        
        timer_man.clear();
        greeter->restart();
        exchanger->restart();
        device.rec.status = Remote_status::Disconnected;
        /* Try to greet the remote */
        greeter->greet();
    }

}