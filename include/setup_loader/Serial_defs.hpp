#pragma once

#include <string>
#include <string_view>

class Serial_defs
{
    static constexpr std::string_view true_s{"true"};
    static constexpr std::string_view false_s{"false"};

public:
    bool bool_trans(const std::string& settings_known);
    bool settings_known_trans(const std::string& settings_known);
    bool write_access_trans(const std::string& write_access);

    Serial_defs() = default;
    Serial_defs(Serial_defs&&) = default;
    Serial_defs& operator=(Serial_defs&&) = default;
    Serial_defs(const Serial_defs&) = default;
    Serial_defs& operator=(const Serial_defs&) = default;
    virtual ~Serial_defs() = 0;
};
