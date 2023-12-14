#pragma once

#include <Base_serial_ctrl.hpp>
#include <impl/Controller.hpp>
#include <Mqtt_defs.hpp>
#include <Dispacher.hpp>
#include <Ip_master.hpp>
#include <Num_except.hpp>
#include <array>

namespace Serial_port
{
    class Serial;
}

namespace Phy_serial
{
    class Console;

    class Serial_ctrl : public Base_serial_ctrl, 
                        protected Ip_serial::Mqtt_defs, 
                        // protected Ip_master, 
                        protected Ip_serial::Ip_packet,
                        public std::enable_shared_from_this<Serial_ctrl>
    {
    public:
        using Serial_ptr = std::shared_ptr<Serial_port::Serial>;
        // using Serial_Ctrl_pair = std::pair<std::unique_ptr<Serial_port::Serial>, std::shared_ptr<Serial_ctrl>>;

    private:
        Mqtt_port::Impl::Controller &controller; // 2
        const std::string com1;                  // 1
        const std::string com2;                  // 1
        bool write_access;                       // 1
        const std::string channel_name;          // 1
        Serial_ptr serial;

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
        auto id = master_counter->num_up();
        
        controller.write(
            channel_name,
            qos,
            std::string{packet_num_s},
            std::to_string(id),
            begin,
            end,
            [serial_ = shared_from_this(), this, id](size_t) {
                if (master_counter->get_not_acked() > max_not_ack)
                {
                    Monitor::get().error(Exception::Num_except{"Device did not send packet ack."});
                }
                
                /* Make entity accessable again */
                msgs->unmark(id);
                
                /* Set message number */
                auto &msg = msgs->first_free();
                msg.id = master_counter->exp();
                
                // Unlock serial's buffer
                serial->listen<Serial_port::Serial::Data>(msg.data.begin(), msg.max_size);
            },
            [](int)
            {
                // send error to monitor
            });
        flow += end - begin;
        Monitor::get().wake(device);
    }
}