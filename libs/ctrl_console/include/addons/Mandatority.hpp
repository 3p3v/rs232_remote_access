#pragma once

namespace Cmd_ctrl
{
    class Mandatority
    {
    public:
        enum class Type
        {
            mandatory,
            optional
        };

    private:    
        bool executed{false};
        Type type{Type::optional};

    public:
        void set_mandatority(Type type);
        void done();
        bool check();
    };
}