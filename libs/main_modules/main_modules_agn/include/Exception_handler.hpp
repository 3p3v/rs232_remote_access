#pragma once

#include <stdexcept>
#include <Error_observer.hpp>
#include <Dev_remover.hpp>
#include <Dev_getter.hpp>
#include <Notyfication_forwarder.hpp>
#include <mutex>

namespace Logic
{
    /// @brief Class handling exceptions from devices // TODO output messages to errod_debug, instead of debug
    class Exception_handler : public Error_observer
    {
        std::unique_ptr<Dev_remover> remover;
        Dev_getter getter;
        Notyfication_forwarder forwarder;

        bool close_on_timeout;
        bool close_on_data_loss;
        bool close_on_protocol_error;

        /// @brief Funcion handling exceptions
        /// @param num Number of a device passing exception
        /// @param e Exception thrown
        void handle_exception(Dev_num num, const std::exception &e);

    public:
        /// @brief
        /// @param forwarder Forwarding debug messages
        /// @param remover Interface for removing devices
        /// @param getter Interface for getting
        /// @param close_on_timeout
        /// @param close_on_data_loss
        /// @param close_on_protocol_error
        template <
            typename Dev_remover_t,
            typename = std::enable_if_t<
                std::is_base_of_v<
                    Dev_remover,
                    std::decay_t<Dev_remover_t>>>>
        Exception_handler(
            Notyfication_forwarder &&forwarder,
            Dev_remover_t &&remover,
            Dev_getter &&getter,
            bool close_on_timeout,
            bool close_on_data_loss,
            bool close_on_protocol_error);
    };

    template <
        typename Dev_remover_t,
        typename>
    Exception_handler::Exception_handler(
        Notyfication_forwarder &&forwarder,
        Dev_remover_t &&remover,
        Dev_getter &&getter,
        bool close_on_timeout,
        bool close_on_data_loss,
        bool close_on_protocol_error)
        : Error_observer{
              [this](Error_job &job)
              {
                  handle_exception(job.num, job.get_exc());
              }},
          forwarder{std::move(forwarder)}, 
          remover{std::make_unique<std::decay_t<Dev_remover_t>>(std::forward<Dev_remover_t>(remover))}, 
          getter{std::move(getter)}, 
          close_on_timeout{close_on_timeout}, 
          close_on_data_loss{close_on_data_loss}, 
          close_on_protocol_error{close_on_protocol_error}
    {
    }
}