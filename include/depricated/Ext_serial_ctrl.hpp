#pragma once

#include <Base_serial_ctrl.hpp>

namespace Phy_serial
{
    class Ext_serial_ctrl : public Base_serial_ctrl
    {
        

    public:
        std::pair<const std::string &, const std::string &> get_coms() const;

        template <typename Str1, typename Str2>
        Ext_serial_ctrl(Base_serial_ctrl base,
                        );
        Ext_serial_ctrl(Ext_serial_ctrl &&) = default;
        Ext_serial_ctrl &operator=(Ext_serial_ctrl &&) = default;
        Ext_serial_ctrl(const Ext_serial_ctrl &) = delete;
        Ext_serial_ctrl &operator=(const Ext_serial_ctrl &) = delete;
        ~Ext_serial_ctrl();
    };

    
}