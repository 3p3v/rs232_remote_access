#pragma once

#include <Port_settings.hpp>

namespace Logic
{
    class Serial_dummy
    {
        /********************************** Settings  **********************************/
    public:
        void set_baud_rate(Port_settings::Baud_rate baud_rate)
        {
        }

        void set_parity(Port_settings::Parity parity)
        {
        }

        void set_char_size(Port_settings::Char_size char_size)
        {
        }

        void set_stop_bits(Port_settings::Stop_bits stop_bits)
        {
        }

        /********************************** Data  **********************************/
    public:
        /// @brief Write to serial, use callbacks
        /// @tparam Iter_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param begin
        /// @param end
        /// @param ok_callb
        /// @param ec_callb
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
        }

        /// @brief Connect to serial
        /// @tparam Iter_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param begin
        /// @param end
        /// @param ok_callb
        /// @param ec_callb
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void connect(Iter_t begin,
                     Iter_t end,
                     Ok_callb &&ok_callb,
                     Ec_callb &&ec_callb)
        {
        }
    };
}