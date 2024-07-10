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
    using Mqtt_msg_t = Mqtt_msg<Val_t>;

private:
    std::array<Mqtt_msg_t, max_saved> msgs;

public:
    /// @brief Get msg with given id, if not found throws an exception
    /// @param id
    /// @return
    Mqtt_msg_t &operator[](Val_t id);

    /// @brief Get all message indexes that are >= id
    /// @param id
    /// @return
    auto get(Val_t id) noexcept;

    /// @brief Find first free message, if not found throws exception
    /// @param id
    /// @return
    Mqtt_msg_t &first_free(Val_t id);

    /// @brief Get oldest message, if no more unused messages exist throws error
    /// @param id
    /// @return
    Mqtt_msg_t &oldest(Val_t id);

    /// @brief Free all messages that have id < argument
    /// @param id
    void free_untill(Val_t id) noexcept;

    /// @brief Mark all messages as uninited
    void reload() noexcept;
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
    auto msg = std::find_if(
        msgs.begin(),
        msgs.end(),
        [this, id](auto &&c)
        {
            if (c.id_ == id) // && c.used == false)
                return true;
            else
                return false;
        });

    if (msg == msgs.end())
        throw std::logic_error{"Packet: " + std::to_string(id) + " does not exist!"};

    msg->used = true;
    msg->inited = true;

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
    T3>::get(Val_t id) noexcept
{
    auto max_id = id + max_saved - 1;

    if (max_id <= max_msg_num)
    {
        auto msg_idx = std::vector<Val_t>();

        std::for_each(
            msgs.begin(),
            msgs.end(),
            [this, id, &msg_idx](auto &&c)
            {
                if ((c.id_ >= id && c.id_ <= max_id) && c.used == false && c.inited == true)
                {
                    c.used = true;
                    msg_idx.emplace_back(c.id_);
                }
            });

        std::sort(msg_idx.begin(), msg_idx.end());

        return msg_idx;
    }
    else
    {
        auto msg_idx_end = std::vector<Val_t>();
        auto msg_idx_beg = std::vector<Val_t>();
        
        auto end = max_id - max_msg_num;

        std::for_each(
            msgs.begin(),
            msgs.end(),
            [this, id, end, &msg_idx_end, &msg_idx_beg](auto &&c)
            {
                if (c.used == false && c.inited == true)
                {
                    if (c.id_ >= id)
                    {
                        c.used = true;
                        msg_idx_end.emplace_back(c.id_);
                    }
                    else if (c.id_ <= end)
                    {
                        c.used = true;
                        msg_idx_beg.emplace_back(c.id_);
                    }
                }
            });

        std::sort(msg_idx_end.begin(), msg_idx_end.end());
        std::sort(msg_idx_beg.begin(), msg_idx_beg.end());
        
        msg_idx_end.insert(
            msg_idx_end.end(), 
            std::make_move_iterator(msg_idx_beg.begin()), 
            std::make_move_iterator(msg_idx_beg.end()));

        return msg_idx_end;
    }
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
    msg->inited = true;

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

    /* Normalization */
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

            if (o_id < u_id && c.used == false)
            {
                msg = iter;
            }

            iter++;
        });

    msg->id_ = id;
    msg->freed = false;
    msg->used = true;
    msg->inited = true;

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
    auto min_id = id - max_saved + 1;

    if (min_id >= min_msg_num)
    {
        std::for_each(
            msgs.begin(),
            msgs.end(),
            [this, id](auto &&c)
            {
                if ((c.id_ <= id && c.id_ >= min_id) && c.used == false)
                {
                    c.freed = true;
                }
            });
    }
    else
    {
        auto beg = min_id + max_msg_num;

        std::for_each(
            msgs.begin(),
            msgs.end(),
            [this, id, beg](auto &&c)
            {
                if ((c.id_ >= beg || c.id_ <= id) && c.used == false)
                {
                    c.freed = true;
                }
            });
    }
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
    T3>::reload() noexcept
{
    std::for_each(
        msgs.begin(),
        msgs.end(),
        [this](auto &c)
        {
            c.inited = false;
        });
}
