#pragma once

#include <Mqtt_msg_cont_base.hpp>
#include <Mqtt_msg.hpp>
#include <algorithm>
#include <stdexcept>

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    std::make_unsigned_t<Val_t> max_saved,
    typename = typename std::enable_if_t<
        std::is_same_v<Val_t, std::decay_t<Val_t>>>,
    typename = typename std::enable_if_t<
        (min_msg_num < max_msg_num)>,
    typename = typename std::enable_if_t<
        ((2 * (max_msg_num - min_msg_num)) > max_saved)>>
class Mqtt_msg_cont : Mqtt_msg_cont_base
{
public:
    using Msg = Mqtt_msg<Val_t>;

private:
    std::array<Msg, max_saved> msgs;

public:
    /// @brief Get msg with given id, if not found throws exception
    /// @param id
    /// @return
    Msg &operator[](Val_t id);

    /// @brief Get all message references that are >= id
    /// @param id
    /// @return
    auto get(Val_t id);

    /// @brief Find first free message, if not found throws exception
    /// @param id
    /// @return
    Msg &first_free(Val_t id);

    /// @brief Get oldest message, if no more unused messages exist throws error
    /// @param id
    /// @return
    Msg &oldest(Val_t id);

    /// @brief Free all messages that have id < argument
    /// @param id
    void free_untill(Val_t id) noexcept;
};

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    std::make_unsigned_t<Val_t> max_saved,
    typename T1,
    typename T2,
    typename T3>
inline Mqtt_msg<Val_t> &Mqtt_msg_cont<
    Val_t,
    min_msg_num,
    max_msg_num,
    max_saved,
    T1,
    T2,
    T3>::operator[](Val_t id)
{
    auto msg = std::find_if(msgs.begin(),
                            msgs.end(),
                            [this, id](auto &&c)
                            {
                                if (c.id_ == id && c.used == false)
                                    return true;
                                else
                                    return false;
                            });

    if (msg == msgs.end())
        throw std::logic_error{"Packet: " + std::to_string(id) + " does not exist!"};

    msg->used = true;
    return *msg;
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    std::make_unsigned_t<Val_t> max_saved,
    typename T1,
    typename T2,
    typename T3>
inline auto Mqtt_msg_cont<
    Val_t,
    min_msg_num,
    max_msg_num,
    max_saved,
    T1,
    T2,
    T3>::get(Val_t id)
{
    std::vector<Mqtt_msg<Val_t> &> msgs{};

    if ((id + max_saved) <= max_msg_num)
    {
        std::for_each(msgs.begin(),
                      msgs.end(),
                      [this, id, &msgs](auto &&c)
                      {
                          if ((c.id_ >= id || c.id_ <= (id + max_saved)) && c.used == false)
                          {
                              c.used = true;
                              msgs.emplace_back(c);
                          }
                      });
    }
    else
    {
        auto end = (id + max_saved) % max_msg_num;

        std::for_each(msgs.begin(),
                      msgs.end(),
                      [this, id, beg, &msgs](auto &&c)
                      {
                          if ((c.id_ >= id || c.id_ < end) && c.used == false)
                          {
                              c.used = true;
                              msgs.emplace_back(c);
                          }
                      });
    }

    return msgs;
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    std::make_unsigned_t<Val_t> max_saved,
    typename T1,
    typename T2,
    typename T3>
inline Mqtt_msg<Val_t> &Mqtt_msg_cont<
    Val_t,
    min_msg_num,
    max_msg_num,
    max_saved,
    T1,
    T2,
    T3>::first_free(Val_t id)
{
    auto msg = std::find_if(msgs.begin(),
                            msgs.end(),
                            [this](auto &&c)
                            {
                                return static_cast<bool>(c.freed) & (!static_cast<bool>(c.used));
                            });

    if (msg == msgs.end())
        throw std::logic_error{"No more free msgs!"};

    msg->id_ = id;
    msg->freed = false;
    msg->used = true;

    return *msg;
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    std::make_unsigned_t<Val_t> max_saved,
    typename T1,
    typename T2,
    typename T3>
inline Mqtt_msg<Val_t> &Mqtt_msg_cont<
    Val_t,
    min_msg_num,
    max_msg_num,
    max_saved,
    T1,
    T2,
    T3>::oldest(Val_t id)
{
    auto msg = std::find_if_not(
        msgs.begin(),
        msgs.end(),
        [](auto &c)
        {
            return static_cast<bool>(c.used);
        });

    if (msg == msgs.end())
        throw std::logic_error{"No more not used msgs!"};

    auto iter = msgs.begin();

    /* Normalisation */
    auto offset = max_msg_num - (id - 1);

    std::for_each(
        msgs.begin(),
        msgs.end(),
        [&, this](auto &c)
        {
            Val_t o_id;
            Val_t u_id;

            if (c.id_ <= id)
            {
                o_id = c.id_ + offset;
            }
            else
            {
                o_id = ((c.id_ + offset) % max_msg_num) + min_msg_num;
            }

            if (msg->id_ <= id)
            {
                u_id = msg->id_ + offset;
            }
            else
            {
                u_id = ((msg->id_ + offset) % max_msg_num) + min_msg_num;
            }

            std::cout << "used: " << std::to_string(msg->id_) << ", new: " << std::to_string(c.id_) << '\n';
            std::cout << "used: " << std::to_string(u_id) << ", new: " << std::to_string(o_id) << '\n';

            if (o_id < u_id && c.used == false)
            {
                std::cout << "CHANGE!" << '\n';
                msg = iter;
            }

            iter++;
        });

    std::cout << "MSG_NUM: " << std::to_string(msg->id_) << '\n';
    msg->id_ = id;
    msg->freed = false;
    msg->used = true;

    return *msg;
}

template <
    typename Val_t,
    Val_t min_msg_num,
    Val_t max_msg_num,
    std::make_unsigned_t<Val_t> max_saved,
    typename T1,
    typename T2,
    typename T3>
inline void Mqtt_msg_cont<
    Val_t,
    min_msg_num,
    max_msg_num,
    max_saved,
    T1,
    T2,
    T3>::free_untill(Val_t id) noexcept
{
    // if (std::find(msgs.cbegin(), msgs.cend(), id) != msgs.cend())
    // {
    if ((id - max_saved) >= min_msg_num)
    {
        std::for_each(msgs.begin(),
                      msgs.end(),
                      [this, id](auto &&c)
                      {
                          if (c.id_ <= id && c.used == false)
                          {
                              c.freed = true;
                          }
                      });
    }
    else
    {
        auto beg = max_msg_num - (id % min_msg_num);

        std::for_each(msgs.begin(),
                      msgs.end(),
                      [this, id, beg](auto &&c)
                      {
                          if ((c.id_ > beg || c.id_ <= id) && c.used == false)
                          {
                              c.freed = true;
                          }
                      });
    }
    // }
}