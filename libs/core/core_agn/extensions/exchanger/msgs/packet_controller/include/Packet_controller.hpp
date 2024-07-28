#pragma once

#include <Mqtt_msg_cont.hpp>
#include <Packet_master.hpp>
#include <Packet_controller_except.hpp>
#include <Packet_val.hpp>

namespace Logic
{
    /// @brief Packet container with
    /// @tparam Val_t
    template <typename Val_t>
    class Packet_controller : public Packet_val<Val_t>
    {
        Mqtt_msg_cont<Val_t> cont;

        Packet_master<Val_t> flow;

    public:
        using Mqtt_msg_t = Mqtt_msg<Val_t>;

        Packet_controller(const Packet_sett<Val_t> &ps);

        /// @brief How many messages were not acked
        /// @return
        std::make_unsigned_t<Val_t> get_not_acked();

        /// @brief Get msg with given id, if not found throws exception
        /// @param id
        /// @return
        decltype(auto) operator[](Val_t id);

        auto get(Val_t id);

        /// @brief
        /// @return
        auto &create();

        /// @brief
        /// @return
        auto &oldest();

        /// @brief Free all messages that have id < argument
        /// @param id
        void ack(Val_t id) noexcept;

        void reload() noexcept;
    };

    template <typename Val_t>
    inline Packet_controller<Val_t>::Packet_controller(const Packet_sett<Val_t> &ps)
        : cont{ps}, flow{ps}
    {
    }

    template <typename Val_t>
    inline std::make_unsigned_t<Val_t> Packet_controller<Val_t>::get_not_acked()
    {
        return flow.get_not_acked();
    }

    template <typename Val_t>
    inline decltype(auto) Packet_controller<Val_t>::operator[](Val_t id)
    {
        return cont.operator[](id);
    }

    template <typename Val_t>
    inline auto Packet_controller<Val_t>::get(Val_t id)
    {
        return cont.get(id);
    }

    template <typename Val_t>
    inline auto &Packet_controller<Val_t>::create()
    {
        auto num = flow.num_up();

        try
        {
            return cont.first_free(num);
        }
        catch (const std::logic_error &ec)
        {
            flow.num_down();

            throw Packet_controller_except{ec.what()};
        }
    }

    template <typename Val_t>
    inline auto &Packet_controller<Val_t>::oldest()
    {
        auto num = flow.num_up();

        try
        {
            auto &ret = (cont.oldest(num));
            return ret;
        }
        catch (const std::logic_error &e)
        {
            /* Can't execute, no more space, should never happen */
            flow.num_down();
            throw;
        }

        // return decltype(cont)::Msg{};
    }

    template <typename Val_t>
    inline void Packet_controller<Val_t>::ack(Val_t id) noexcept
    {
        flow.ack(id);
        cont.free_untill(id);
    }

    template <typename Val_t>
    inline void Packet_controller<Val_t>::reload() noexcept
    {
        flow.reload();
        cont.reload();
    }
}