#pragma once

#include <memory>
#include <Base_proto_mediator.hpp>
#include <Notifier.hpp>
#include <Proto_rec.hpp>
#include <Remote_rec.hpp>
#include <Serial_rec.hpp>

using namespace Logic;

namespace Impl
{
    class Base_device_init
    {
    public:
        virtual std::shared_ptr<Base_proto_mediator> create(
            Notifier &&notifier,
            Proto_rec &&rec,
            Remote_rec &&remote_rec,
            Serial_rec &&serial_rec) = 0;

        Base_device_init() = default;
        Base_device_init(Base_device_init &&) = default;
        Base_device_init(const Base_device_init &) = default;
        Base_device_init &operator=(Base_device_init &&) = default;
        Base_device_init &operator=(const Base_device_init &) = default;
        virtual ~Base_device_init() = default;
    };
}