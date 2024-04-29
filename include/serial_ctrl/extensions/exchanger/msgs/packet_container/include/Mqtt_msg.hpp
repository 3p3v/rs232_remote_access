#pragma once

#include <array>
#include <atomic>
#include <cstddef>

class Mqtt_msg_cont_base;

template <typename Val_t = unsigned char>
class Mqtt_msg
{
public:
    static constexpr size_t max_size{1300};

    using Cont_t = std::array<unsigned char, max_size>;

public:
    friend Mqtt_msg_cont_base;

    Cont_t data;
    size_t data_len;
    Val_t id_;
    /* If data is safe to be freed */
    std::atomic_bool freed{true};
    /* If data is being changed now */
    std::atomic_bool used{false};
    /// @brief If contains information that is not garbage
    std::atomic_bool inited{false};

public:
    typename Cont_t::iterator begin();
    typename Cont_t::iterator end();
    void len(size_t len);
    void unused() noexcept;
    void uninit() noexcept;
    Val_t id() const;
    bool operator==(const Mqtt_msg<Val_t>& msg) const;
    bool operator==(const Val_t& id) const;
};

template <typename Val_t>
typename Mqtt_msg<Val_t>::Cont_t::iterator Mqtt_msg<Val_t>::begin()
{
    return data.begin();
}

template <typename Val_t>
typename Mqtt_msg<Val_t>::Cont_t::iterator Mqtt_msg<Val_t>::end()
{
    return data.end();
}

template <typename Val_t>
void Mqtt_msg<Val_t>::len(size_t len)
{
    data_len = len;
}

template <typename Val_t>
void Mqtt_msg<Val_t>::unused() noexcept
{
    used = false;
}

template <typename Val_t>
inline void Mqtt_msg<Val_t>::uninit() noexcept
{
    inited = false;
}

template <typename Val_t>
Val_t Mqtt_msg<Val_t>::id() const
{
    return id_;
}

template <typename Val_t>
bool Mqtt_msg<Val_t>::operator==(const Mqtt_msg<Val_t> & msg) const
{
    return id_ == msg.id_;
}

template <typename Val_t>
bool Mqtt_msg<Val_t>::operator==(const Val_t &id) const
{
    return id_ == id;
}