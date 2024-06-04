#pragma once

#include <Serial_base.hpp>

namespace Logic
{
    /// @brief Serial data interface.
    ///        Declares all methods needed by the functional class to be suted for communication.
    /// @tparam Impl_t Class implementing functions.
    template <typename Impl_t>
    class Serial_side_connect : public Serial_base<Impl_t>
    {
        using Serial_base_t::Serial_base;

    public:
        template <typename Iter_t, typename Ok_callb, typename Ec_callb>
        void connect(
            Iter_t begin,
            Iter_t end,
            Ok_callb &&ok_callb,
            Ec_callb &&ec_callb);
    };

    // template <typename Impl_t>
    // Serial_side_connect(Impl_t &&) -> Serial_side_connect<Impl_t>;

    template <typename Impl_t>
    template <typename Iter_t, typename Ok_callb, typename Ec_callb>
    inline void Serial_side_connect<Impl_t>::connect(
        Iter_t begin,
        Iter_t end,
        Ok_callb &&ok_callb,
        Ec_callb &&ec_callb)
    {
        if (auto i = get_impl())
            i->connect(
            begin,
            end,
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }
}