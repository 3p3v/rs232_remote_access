#pragma once

#include <Setup_impl.hpp>
#include <sstream>

using namespace Impl;

class Setup_deb final : public Setup_impl
{
public:
    Data_pack load_data(const std::string &setup) &&;
};

inline Setup_deb::Data_pack Setup_deb::load_data(const std::string &setup) &&
{
    std::istringstream iss{setup};
    std::string line;
    
    while(std::getline(iss, line))
    {
        console.exec(line.cbegin(), line.cend());
    }

    return std::move(data_pack);
}