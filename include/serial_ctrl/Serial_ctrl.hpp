#pragma once

#include <Base_serial_ctrl.hpp>
#include <impl/Controller.hpp>
#include <Mqtt_defs.hpp>

namespace Serial_port
{
    class Serial;
}

namespace Phy_serial
{
    class Console;

    class Serial_ctrl : public Base_serial_ctrl, protected Ip_serial::Mqtt_defs, public std::enable_shared_from_this<Serial_ctrl>
    {
    public:
        using Serial_ptr = std::unique_ptr<Serial_port::Serial>;
        // using Serial_Ctrl_pair = std::pair<std::unique_ptr<Serial_port::Serial>, std::shared_ptr<Serial_ctrl>>;

    private:
        Mqtt_port::Impl::Controller &controller; // 2
        const std::string com1;                        // 1
        const std::string com2;                        // 1
        bool write_access;                             // 1
        const std::string channel_name;                // 1

    public:
        /// @brief Write to mqtt controller
        /// @tparam Iter_t 
        /// @param begin 
        /// @param end 
        template <typename Iter_t>
        void write(const Iter_t begin, const Iter_t end);

        /// @brief Connect to virtual port
        /// @return 
        Serial_ptr connect();

        /// @brief Get virtual COM ports
        /// @return 
        std::pair<const std::string &, const std::string &> get_coms() const;

        template <typename Str1, typename Str2>
        Serial_ctrl(Base_serial_ctrl base,
                    Mqtt_port::Impl::Controller &controller,
                    Str1 &&com1,
                    Str2 &&com2,
                    bool write_access);
    };

    template <typename Str1, typename Str2>
    Serial_ctrl::Serial_ctrl(Base_serial_ctrl base,
                             Mqtt_port::Impl::Controller &controller,
                             Str1 &&com1,
                             Str2 &&com2,
                             bool write_access)
        : Base_serial_ctrl{std::move(base)},
          controller{controller},
          channel_name{device->get_data_ch()},
          com1{std::forward<Str1>(com1)},
          com2{std::forward<Str2>(com2)},
          write_access{write_access}

    {
    }

    template <typename Iter_t>
    void Serial_ctrl::write(const Iter_t begin, const Iter_t end)
    {
        controller.write(channel_name, qos, begin, end);
        flow += end - begin;
        monitor.wake(device);
    }
}