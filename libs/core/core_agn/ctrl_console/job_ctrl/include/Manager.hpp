#pragma once

#include <Storage.hpp>
#include <Forwarder.hpp>

namespace Job_ctrl
{
    class Manager_dummy
    {
    public:
        Worker_storage ws{};

        Manager_dummy() = default;
        Manager_dummy(Manager_dummy&&) = default;
        Manager_dummy& operator=(Manager_dummy&&) = default;
        Manager_dummy(const Manager_dummy &) = default;
        Manager_dummy& operator=(const Manager_dummy &) = default;
        virtual ~Manager_dummy() = 0;
    };

    inline Manager_dummy::~Manager_dummy() = default;

    class Manager
        : Manager_dummy,
          public Storage,
          public Forwarder
    {
    public:
        Manager();
    };

    inline Manager::Manager()
        : Storage{Manager_dummy::ws}, Forwarder{Manager_dummy::ws}
    {
    }
}