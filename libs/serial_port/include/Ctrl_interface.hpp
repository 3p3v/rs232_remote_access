#pragma once

namespace Serial_port
{
    class Ctrl_defs
    {
    public:
        enum class Parity
        {
            none,
            odd,
            even
        };

        enum class Flow_ctrl
        {
            none,
            software,
            hardware
        };

        enum class Stop_bits
        {
            one,
            onepointfive,
            two
        };
    };
    
    class Ctrl_interface : public Ctrl_defs
    {
    public:
        unsigned int baud_rate{9600};
        Parity parity{Parity::none};
        unsigned char char_size{8};
        Flow_ctrl flow_ctrl{Flow_ctrl::none};
        Stop_bits stop_bits{Stop_bits::one};

        virtual void set_baud_rate(const unsigned int baud_rate) = 0;
        virtual void set_parity(const Parity parity) = 0;
        virtual void set_char_size(unsigned int char_size) = 0;
        virtual void set_flow_ctrl(const Flow_ctrl flow_ctrl) = 0;
        virtual void set_stop_bits(const Stop_bits stop_bits) = 0;
        virtual void set_baud_rate() = 0;
        virtual void set_parity() = 0;
        virtual void set_char_size() = 0;
        virtual void set_flow_ctrl() = 0;
        virtual void set_stop_bits() = 0;
    };
}