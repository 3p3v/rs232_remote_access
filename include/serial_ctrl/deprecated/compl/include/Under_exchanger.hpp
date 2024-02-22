#pragma once 

namespace Logic
{
    class Under_exchanger
    {
    public:
        /// @brief Resend all messages >= id
        /// @param id 
        virtual void resend(char id) = 0;

        Under_exchanger() = default;
        Under_exchanger(Under_exchanger &&) = default;
        Under_exchanger(const Under_exchanger &) = delete;
        Under_exchanger& operator=(Under_exchanger &&) = default;
        Under_exchanger& operator=(const Under_exchanger &) = delete;
        virtual ~Under_exchanger() = default;
    };
    
    Under_exchanger::Under_exchanger(/* args */)
    {
    }
    
    Under_exchanger::~Under_exchanger()
    {
    }
    
}