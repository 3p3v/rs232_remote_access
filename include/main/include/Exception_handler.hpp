#pragma once

#include <stdexcept>
#include <Error_observer.hpp>
#include <Dev_remover.hpp>
#include <Notyfication_forwarder.hpp>

namespace Logic
{
    class Exception_handler : public Error_observer
    {
        Dev_remover controller;
        Notyfication_forwarder &forwarder;
        
        void handle_exception(Dev_num num, const std::exception &e);

    public:
        Exception_handler(Notyfication_forwarder &forwarder, Dev_remover &&controller);
    };
}