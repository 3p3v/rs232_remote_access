#pragma once

#include <Proto_rec.hpp>
#include <Remote_rec.hpp>
#include <Serial_rec.hpp>

namespace Logic
{
    class Base_proto_mediator
    {
    public:
        /// @brief Start communication
        virtual void start() = 0;
        /// @brief Restart communication
        virtual void restart() = 0;

        /// @brief Get protocol data
        virtual Proto_rec& get_rec() const noexcept = 0; // TODO move helpers to proto_mediator and change to non-virtual
        /// @brief Get MQTT startup information
        virtual Remote_rec& get_remote_rec() const noexcept = 0; // TODO move helpers to proto_mediator and change to non-virtual
        /// @brief Get serial startup information
        virtual Serial_rec& get_serial_rec() const noexcept = 0; // TODO move helpers to proto_mediator and change to non-virtual

        Base_proto_mediator() = default;
        Base_proto_mediator(Base_proto_mediator &&) = delete;
        Base_proto_mediator(const Base_proto_mediator &) = delete;
        Base_proto_mediator &operator=(Base_proto_mediator &&) = delete;
        Base_proto_mediator &operator=(const Base_proto_mediator &) = delete;
        virtual ~Base_proto_mediator() = default;
    };
}