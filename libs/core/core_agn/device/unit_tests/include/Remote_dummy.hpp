#pragma once

namespace Logic
{
    class Remote_dummy
    {
    public:
        /// @brief Write command with an argument
        /// @tparam Cont_t
        /// @tparam Arg_cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param msg
        /// @param arg
        /// @param ok_callb
        /// @param ec_callb
        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        void write_i(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
        }

        /// @brief Write command with no arguments
        /// @tparam Cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param msg
        /// @param ok_callb
        /// @param ec_callb
        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        void write_i(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
        }

        /// @brief Write command with an argument
        /// @tparam Cont_t
        /// @tparam Arg_cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param msg
        /// @param arg
        /// @param ok_callb
        /// @param ec_callb
        template <typename Cont_t, typename Arg_cont_t, typename Ok_callb, typename Ec_callb>
        void write_s(Cont_t &&msg, Arg_cont_t &&arg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
        }

        /// @brief Write command with no arguments
        /// @tparam Cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param msg
        /// @param ok_callb
        /// @param ec_callb
        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        void write_s(Cont_t &&msg, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
        }

        /// @brief Connect to info channel, set callbacks
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param ok_callb
        /// @param ec_callb
        template <typename Ok_callb, typename Ec_callb>
        void connect_i(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
        }

        /********************************** Data  **********************************/
    public:
        /// @brief Write to device, use callbacks
        /// @tparam Cont_t
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param id
        /// @param begin
        /// @param end
        /// @param ok_callb
        /// @param ec_callb
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void write(char id, Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
        }

        /// @brief Read from device to buffer, use callbacks
        /// @tparam Ok_callb
        /// @tparam Ec_callb
        /// @param ok_callb
        /// @param ec_callb
        template <typename Ok_callb, typename Ec_callb>
        void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
        {
        }
    };
}