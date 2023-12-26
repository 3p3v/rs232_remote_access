#pragma once

namespace Logic
{
    /// @brief MQTT data interface.
    ///        Declares all methods needed by the functional class to be suted for communication.
    /// @tparam Impl Class implementing functions.
    template <typename Impl>
    class Mqtt_side
    {
        Impl impl;

    public:
        template <typename Cont_t, typename Ok_callb, typename Ec_callb>
        void write(char id, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        template <typename Ok_callb, typename Ec_callb>
        void connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb);

        // Mqtt_side(const Impl &impl);
        Mqtt_side(Impl &&impl);
    };

    template <typename Impl>
    Mqtt_side(Impl &&) -> Mqtt_side<Impl>;

    template <typename Impl>
    template <typename Cont_t, typename Ok_callb, typename Ec_callb>
    inline void Mqtt_side<Impl>::write(char id, const typename Cont_t::const_iterator begin, const typename Cont_t::const_iterator end, Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        impl.write<Cont_t>(
            id,
            begin,
            end,
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb))
    }

    template <typename Impl>
    template <typename Ok_callb, typename Ec_callb>
    inline void Mqtt_side<Impl>::connect(Ok_callb &&ok_callb, Ec_callb &&ec_callb)
    {
        impl.connect(
            std::forward<Ok_callb>(ok_callb),
            std::forward<Ec_callb>(ec_callb));
    }

    // template <typename Impl>
    // inline Mqtt_side<Impl>::Mqtt_side(const Impl &impl)
    //     : Mqtt_side{impl}
    // {
    // }

    template <typename Impl>
    inline Mqtt_side<Impl>::Mqtt_side(Impl &&impl)
        : impl{std::move(impl)}
    {
    }
}