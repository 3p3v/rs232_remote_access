#pragma once

#include <stdexcept>
#include <Error_observer.hpp>
#include <Dev_remover.hpp>
#include <Dev_getter.hpp>
#include <Notyfication_forwarder.hpp>
#include <mutex>

namespace Logic
{
    class Exception_handler : public Error_observer
    {   
        Dev_remover remover;
        Dev_getter getter;
        Notyfication_forwarder forwarder;

        bool close_on_timeout;
        bool close_on_data_loss;
        bool close_on_protocol_error;

        void handle_exception(Dev_num num, const std::exception &e);

    public:
        Exception_handler(
            Notyfication_forwarder &&forwarder,
            Dev_remover &&remover,
            Dev_getter &&getter,
            bool close_on_timeout,
            bool close_on_data_loss,
            bool close_on_protocol_error);
    };
}