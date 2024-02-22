#pragma once

#include <string>
#include <functional>

namespace Logic
{
    class Base_setter
    {
    protected:
        virtual void write_i(const char *cmd, std::string &&arg) = 0;
        virtual void write_i(const char *cmd, std::string &&arg, std::function<void()> &&ok_callb) = 0;
        virtual void write_i(const char *cmd, std::function<void()> &&ok_callb) = 0;
        
    public:
        Base_setter() = default;
        Base_setter(Base_setter &&) = default;
        Base_setter(const Base_setter &) = delete;
        Base_setter& operator=(Base_setter &&) = default;
        Base_setter& operator=(const Base_setter &) = delete;
        virtual ~Base_setter() = default;
    }; 
}