#pragma once

namespace Logic
{
    /// @brief Serial data interface.
    ///        Declares all methods needed by the functional class to be suted for communication.
    /// @tparam Impl Class implementing functions.
    template <typename Impl>
    class Serial_side_connect
    {
        Impl impl;

    public:
        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        void connect(
            const typename Cont_t::const_iterator begin,
            const typename Cont_t::const_iterator end,
            Ok_callb &&ok_callb,
            Ec_callb &&ec_callb);

        Serial_side_connect(Impl &&impl);
    };

    template <typename Impl>
    Serial_side_connect(Impl &&) -> Serial_side_connect<Impl>;

    template <typename Impl>
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Serial_side_connect<Impl>::connect(
        const typename Cont_t::const_iterator begin,
        const typename Cont_t::const_iterator end,
        Ok_callb &&ok_callb,
        Ec_callb &&ec_callb)
    {
        impl.connect(
            begin,
            end,
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }

    template <typename Impl>
    inline Serial_side_connect<Impl>::Serial_side_connect(Impl &&impl)
    {
    }
}