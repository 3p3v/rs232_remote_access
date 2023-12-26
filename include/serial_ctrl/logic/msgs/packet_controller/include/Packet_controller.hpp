#pragma once

#include <Mqtt_msg_cont.hpp>
#include <Packet_master.hpp>

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
        /// @brief Get msg with given id, if not found throws exception
        /// @param id
        /// @return
        decltype(auto) operator[](Val_t id);

        /// @brief Create new message, if no more unused messages exist throws error
        /// @tparam Ec_callb
        /// @param ec_callb
        /// @return
        template <typename Ec_callb>
        Mqtt_msg<Val_t> &create(Ec_callb &&ec_callb);

        decltype(auto) create();

        /// @brief Free all messages that have id < argument
        /// @param id
        void ack(Val_t id) noexcept;
    };

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline decltype(auto) Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::operator[](Val_t id)
    {
        return cont.operator[](id);
    }

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline decltype(auto) Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::create()
    {
        return create([]() {});
    }

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    inline void Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::ack(Val_t id) noexcept
    {
        flow.ack(id);
        cont.free_untill(id);
    }

    template <typename Val_t, Val_t min_msg_num, Val_t max_msg_num, std::make_unsigned_t<Val_t> max_saved>
    template <typename Ec_callb>
    inline Mqtt_msg<Val_t> &Packet_controller<Val_t, min_msg_num, max_msg_num, max_saved>::create(Ec_callb &&ec_callb)
    {
        auto num = flow.num_up();

        try
        {
            // std::cout << "TEST0" << '\n';
            return cont.first_free(num);
        }
        catch (const std::logic_error &ec)
        {
            try
            {
                // std::cout << "TEST1" << '\n';
                auto &ret = (cont.oldest(num));
                // std::cout << "TEST2" << '\n';
                ec_callb(ec);
                // std::cout << "TEST3" << '\n';
                return ret;
            }
            catch (const std::logic_error &e)
            {
                /* Can't execute, no more space, should never happen */
                flow.num_down();
                throw e;
            }

            return decltype(cont)::Msg{};
        }
    }
}