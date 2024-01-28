#pragma once

#include <memory>
#include <atomic>
#include <sides/Mqtt_settings.hpp>
#include <sides/Serial_settings.hpp>
#include <Timer_cont.hpp>
#include <Periodic_timer.hpp>
#include <Custom_timer.hpp>
#include <Shared_helper.hpp>
#include <Under_setter.hpp>
#include <User_setter.hpp>
#include <Inter_setter.hpp>

using namespace Serial_port;

namespace Logic
{
    /// @brief
    template <typename Mqtt, typename Serial>
    class Setter final : public Under_setter,
                         public User_setter,
                         public Inter_setter,
                         public std::enable_shared_from_this<Setter>
    {
        Mqtt_settings<Mqtt> mqtt;
        Serial_settings<Serial> serial;

        /// @brief Default error callback
        /// @return
        auto def_ec_callb();

    protected:
        std::shared_ptr<Setter> shared_from_this_() override;
        std::weak_ptr<Setter> weak_from_this_() override;
        void write_i(const char *cmd, std::string &&arg) override;
        void write_i(const char *cmd, std::string &&arg, std::function<void()> &&ok_callb) override;
        void write_i(const char *cmd, std::function<void()> &&ok_callb) override;

    public:
        template <typename Mqtt_t,
                  typename Serial_t,
                  typename Shared_helper_ptr_t,
                  typename =
                      std::enable_if_t<
                          !std::is_base_of_v<Mqtt,
                                             std::decay_t<Mqtt_t>>> &&
                      std::enable_if_t<
                          !std::is_base_of_v<Serial,
                                             std::decay_t<Serial_t>>> &&
                      std::enable_if_t<
                          !std::is_same_v<Shared_helper_ptr,
                                          std::decay_t<Shared_helper_ptr_t>>>>
        Setter(Mqtt_t &&mqtt,
               Serial_t &&serial,
               Shared_helper_ptr_t &&helper);
    };

    template <typename Mqtt, typename Serial>
    inline auto Setter<Mqtt, Serial>::def_ec_callb()
    {
        return [](const std::except &e) {};
    }

    template <typename Mqtt, typename Serial>
    inline std::shared_ptr<Setter> Setter<Mqtt, Serial>::shared_from_this_()
    {
        return shared_from_this();
    }

    template <typename Mqtt, typename Serial>
    inline std::weak_ptr<Setter> Setter<Mqtt, Serial>::weak_from_this_()
    {
        return weak_from_this();
    }

    template <typename Mqtt, typename Serial>
    inline void Setter<Mqtt, Serial>::write_i(const char *cmd, std::string &&arg)
    {
        mqtt.write_i(
            cmd, std::move(arg), []() {}, def_ec_callb());
    }

    template <typename Mqtt, typename Serial>
    inline void Setter<Mqtt, Serial>::write_i(const char *cmd, std::string &&arg, std::function<void()> &&ok_callb)
    {
        mqtt.write_i(cmd, std::move(arg), std::move(ok_callb), def_ec_callb());
    }

    template <
        typename Mqtt,
        typename Serial>
    template <
        typename Mqtt_t,
        typename Serial_t,
        typename Shared_helper_ptr_t,
        typename>
    inline Setter<Mqtt, Serial>::Setter(
        Mqtt_t &&mqtt,
        Serial_t &&serial,
        Shared_helper_ptr_t &&heper)
        : mqtt{std::forward<Mqtt_t>(mqtt)},
          serial{std::forward<Serial_t>(serial)},
          helper{std::forward<Shared_helper_ptr_t>(helper)}
    {
    }
}