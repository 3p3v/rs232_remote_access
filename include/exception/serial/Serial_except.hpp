#pragma once

#include <Exception.hpp>
#include <boost/exception/all.hpp>

namespace Exception
{
    class Serial_except : public Exception
    {
    public:
        Serial_except(const std::string &expln);
        Serial_except(const char *expln);
        Serial_except(const boost::exception& except);
    };
}