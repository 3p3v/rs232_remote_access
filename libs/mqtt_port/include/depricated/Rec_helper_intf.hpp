#pragma once 

#include <string>

namespace Mqtt_port
{
    class Rec_helper_intf
    {
    private:
        /* data */
    public:
        Rec_helper_intf();
        Rec_helper_intf(Rec_helper_intf&&);
        Rec_helper_intf& operator=(Rec_helper_intf&&);
        Rec_helper_intf(const Rec_helper_intf&);
        Rec_helper_intf& operator=(const Rec_helper_intf&);
        virtual ~Rec_helper_intf() = default;

        std::string::const_iterator cbegin() = 0;
        std::string::const_iterator cend() = 0;
    };
}