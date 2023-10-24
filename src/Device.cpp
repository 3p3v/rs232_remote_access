#include <Device.hpp>
#include <stdexcept>

std::string Device::get_info_ch()
{
    if (name_set)
        return name + std::string{info_ch};
    else
        throw std::logic_error{"Device name not set!"};
}

std::string Device::get_data_ch()
{
     if (name_set)
        return name + std::string{data_ch};
    else
        throw std::logic_error{"Device name not set!"};
}

const std::string &Device::get_name()
{
    return name;
}
