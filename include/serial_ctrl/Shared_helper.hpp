#pragma once

#include <atomic>
#include <Device.hpp>
#include <Mqtt_msg_cont.hpp>
#include <Ip_master.hpp>
#include <Serial_info.hpp>

/// @brief Common objects for communication in both ways
/// @tparam Mqtt 
/// @tparam Phy 
template <typename Mqtt, typename Phy>
class Shared_helper
{
public:
    /* Message control */
    Ip_serial::Mqtt_msg_cont msgs{}; 
    Ip_serial::Ip_master master_counter{};  

    /* Lock communictaion */
    std::atomic_bool mqtt_lock{true};
    std::atomic_bool phy_lock{true};

    /* Objects implementing communication */
    Mqtt mqtt;
    Phy phy;

    Shared_helper(Mqtt &&mqtt, Phy &&phy); 
};

template <typename Mqtt, typename Phy>
Shared_helper<Mqtt, Phy>::Shared_helper(Mqtt &&mqtt, Phy &&phy)
    : mqtt{std::move(mqtt)}, phy{std::move(phy)}
{
}

