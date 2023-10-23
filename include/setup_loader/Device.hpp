#pragma once

#include <string>
#include <string_view>

class Device final
{
    static constexpr std::string_view info_ch{"_i_ch"};
    static constexpr std::string_view info_ch{"_d_ch"};
    
    std::string name;
    bool name_set{false};

public:
    Device() = default;

    template <typename Str>
    Device(Str &&name);

    template <typename Str>
    void set_name(Str &&name);
    std::string get_info_ch();
    std::string get_data_ch();
};

template <typename Str>
Device::Device(Str &&name)
    : name{std::forward<Str>(name)},
      name_set{true}
{
}

template <typename Str>
void Device::set_name(Str &&name)
{
    name = std::forward<Str>(name);
    name_set = true;
}
