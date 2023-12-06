#include <Serial_except.hpp>

namespace Exception
{
    Serial_except::Serial_except(const std::string &expln)
        : Exception{"SERIAL", expln}
    {
    }  

    Serial_except::Serial_except(const char *expln)
        : Exception{"SERIAL", expln}
    {
    }    

    Serial_except::Serial_except(const boost::exception& except)
        : Exception{"SERIAL", boost::diagnostic_information(except)}
    {

    }
}