#pragma once

#include <array>
#include <atomic>
#include <cstddef>

template <typename Val_t>
class Mqtt_msg_cont;
template <typename Val_t = unsigned char>
class Mqtt_msg
{
    friend Mqtt_msg_cont<Val_t>;

public:
    static constexpr size_t max_size{1300};

    using Cont_t = std::array<unsigned char, max_size>;

private:
    Cont_t data;
    size_t data_len;
    Val_t id_;
    /// @brief If data is safe to be freed 
    bool freed{true};
    /// @brief If data is being changed now 
    bool used{false};
    /// @brief If contains information that is not garbage
    bool inited{false};

public:
    /* Getters */
    typename Cont_t::iterator begin() noexcept;
    typename Cont_t::iterator end() noexcept;
    Val_t id() const noexcept;
    bool if_freed();
    bool if_used();
    bool if_inited();
    size_t get_len();

    /* Setters */
    void set_len(size_t len) noexcept;
    void unused() noexcept;
    void uninit() noexcept;

    bool operator==(const Mqtt_msg<Val_t>& msg) const noexcept;
    bool operator==(const Val_t& id) const noexcept;

    Mqtt_msg() = default;
    Mqtt_msg(Mqtt_msg&&) noexcept;
    Mqtt_msg& operator=(Mqtt_msg<Val_t>&&) noexcept;
    Mqtt_msg(const Mqtt_msg &) = delete;
    Mqtt_msg& operator=(const Mqtt_msg &) = delete;
    ~Mqtt_msg() = default;
};

template <typename Val_t>
typename Mqtt_msg<Val_t>::Cont_t::iterator Mqtt_msg<Val_t>::begin() noexcept
{
    return data.begin();
}

template <typename Val_t>
typename Mqtt_msg<Val_t>::Cont_t::iterator Mqtt_msg<Val_t>::end() noexcept
{
    return data.end();
}

template <typename Val_t>
void Mqtt_msg<Val_t>::set_len(size_t len) noexcept
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
Val_t Mqtt_msg<Val_t>::id() const noexcept
{
    return id_;
}

template <typename Val_t>
inline bool Mqtt_msg<Val_t>::if_freed()
{
    return freed;
}

template <typename Val_t>
inline bool Mqtt_msg<Val_t>::if_inited()
{
    return inited;
}

template <typename Val_t>
inline size_t Mqtt_msg<Val_t>::get_len()
{
    return data_len;
}

template <typename Val_t>
inline bool Mqtt_msg<Val_t>::if_used()
{
    return used;
}

template <typename Val_t>
bool Mqtt_msg<Val_t>::operator==(const Mqtt_msg<Val_t> & msg) const noexcept
{
    return id_ == msg.id_;
}

template <typename Val_t>
bool Mqtt_msg<Val_t>::operator==(const Val_t &id) const noexcept
{
    return id_ == id;
}

template <typename Val_t>
inline Mqtt_msg<Val_t>::Mqtt_msg(Mqtt_msg &&m) noexcept
    : data{std::move(m.data)}, 
      data_len{m.data_len}, 
      id_{m.id_}, 
      freed{m.freed},
      used{m.used},
      inited{m.inited}
{
}

template <typename Val_t>
inline Mqtt_msg<Val_t> &Mqtt_msg<Val_t>::operator=(Mqtt_msg &&m) noexcept
{
    data = std::move(m.data)
    data_len = m.data_len;
    id_ = m.id_;
    freed = m.freed.load();
    used = m.used.load();
    inited = m.inited.load();
    
    return *this;
}
