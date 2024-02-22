#pragma once

#include <sides/Mqtt_side.hpp>
#include <sides/Serial_side.hpp>
#include <Interpreter.hpp>
#include <Timer_cont.hpp>

namespace Logic
{
    /// @brief Record of an equipement
    /// @tparam Mqtt 
    /// @tparam Serial 
    template <typename Mqtt, typename Serial>
    class Remote_record
    {
    public:
        using Console = Base_ctrl_console<const std::string &>, Endl_opt::with>;
        using Remote = Mqtt_side<Mqtt>;
        using Local = Serial_side<Serial>;
        using Timers = Timer_cont;
    
        /// @brief MQTT connection handling 
        Remote mqtt;
        /// @brief Serial port connection handling 
        Local serial;
        /// @brief Lock communictaion 
        std::atomic_bool lock{true};
        /// @brief Generic command interpreter 
        Console console{};
        /// @brief Timers for every other command
        Timers timers{};

        Remote_record(Remote&& mqtt, Local &&serial);
    };

    template <typename Mqtt, typename Serial>
    Remote_record::Remote_record(Remote &&mqtt, Local &&serial)
        : mqtt{std::move(mqtt)}, serial{std::move(serial)}
    {
    }
}