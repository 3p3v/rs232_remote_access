#pragma once

#include <Device_collection.hpp>

using namespace Logic;

namespace Impl
{
    /// @brief Contains all nessesary components to run the application (device containers, info containers, debug, error handling, ect.)
    class Collective_general final
    {
        /// @brief Init of the devices
        Device_collection devices;

    public:
        template <typename... Args_t>
        Collective_general(Args_t &&...args);

        /// @brief Add devices from some container
        /// @tparam Iter_t
        /// @param begin
        /// @param end
        /// @return
        template <typename Iter_t>
        void add_devices(Iter_t begin, Iter_t end) &;
    };

    template <typename... Args_t>
    inline Collective_general::Collective_general(Args_t &&...args)
        devices{std::forward<Args_t>(args), ...}
    {
    }

    template <typename Iter_t>
    inline void Collective_general::add_devices(Iter_t begin, Iter_t end) &
    {
        devices.add_devices(begin, end);
    }
}
