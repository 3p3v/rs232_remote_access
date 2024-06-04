#pragma once

#include <Mqtt_msg_cont.hpp>
#include <Packet_master.hpp>
#include <Packet_controller_except.hpp>

namespace Logic
{
    /// @brief Packet container with
    /// @tparam Val_t
    /// @tparam min_msg_num
    /// @tparam max_msg_num
    /// @tparam max_saved
    template <
        typename Val_t,
        Val_t min_msg_num,
        Val_t max_msg_num,
        std::make_unsigned_t<Val_t> max_saved>
    class Packet_controller
    {
        Mqtt_msg_cont<
            Val_t,
            min_msg_num,
            max_msg_num,
            max_saved>
            cont{};

        Packet_master<
            Val_t,
            min_msg_num,
            max_msg_num>
            flow{};

    public:
        using Mqtt_msg_t = Mqtt_msg<Val_t>;

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

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline std::make_unsigned_t<Val_t> Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::get_not_acked()
    {
        return flow.get_not_acked();
    }

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline decltype(auto) Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::operator[](Val_t id)
    {
        return cont.operator[](id);
    }

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline auto Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::get(Val_t id)
    {
        return cont.get(id);
    }

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline auto &Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::create()
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

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline auto &Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::oldest()
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

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline void Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::ack(Val_t id) noexcept
    {
        flow.ack(id);
        cont.free_untill(id);
    }

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline void Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::reload() noexcept
    {
        flow.reload();
        cont.reload();
    }
}