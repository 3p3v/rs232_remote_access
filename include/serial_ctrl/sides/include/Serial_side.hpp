#pragma once

namespace Logic
{
    /// @brief Serial data interface.
    ///        Declares all methods needed by the functional class to be suted for communication.
    /// @tparam Impl Class implementing functions.
    template <typename Impl>
    class Serial_side
    {
        Impl impl;

    public:
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        Serial_side(Impl &&impl);
    };

    template <typename Impl>
    Serial_side(Impl &&) -> Serial_side<Impl>;

    template <typename Impl>
    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    inline void Serial_side<Impl>::write(Iter_t begin, Iter_t end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        impl.write(
            begin,
            end,
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }

    template <typename Impl>
    inline Serial_side<Impl>::Serial_side(Impl &&impl)
    {
    }
}