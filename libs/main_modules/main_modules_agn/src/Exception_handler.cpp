#include <Exception_handler.hpp>
#include <Intf_except.hpp>
#include <Ext_except.hpp>
#include <Timeout_except.hpp>
#include <Data_loss_except.hpp>
#include <iostream>

using namespace Logic;

void Logic::Exception_handler::handle_exception(Dev_num num, const std::exception &e)
{
    try
    {
        throw e;
    }
    catch (const Intf_except &e)
    {
        forwarder.debug(
            num,
            "Fatal low-level module exception! Shutdown required!\n");
        forwarder.debug(
            num,
            e.what());

        throw;
    }
    catch (const Ext_except &e)
    {
        forwarder.debug(
            num,
            "Protocol error!\n");

        if (close_on_protocol_error)
        {
            forwarder.debug(
                num,
                "Shutting down connection!\n");
            forwarder.debug(
                num,
                e.what());

            remover.remove(num);
        }
        else
        {
            forwarder.debug(
                num,
                "Resetting connection!\n");
            forwarder.debug(
                num,
                e.what());

            auto dev_and_lock = getter.get_and_lock(num);
            dev_and_lock.first.cont.restart();
        }
    }
    catch (const Timeout_except &e)
    {
        forwarder.debug(
            num,
            "Command timed out!\n");

        if (close_on_timeout)
        {
            forwarder.debug(
                num,
                "Shutting down connection!\n");
            forwarder.debug(
                num,
                e.what());

            remover.remove(num);
        }
        else
        {
            forwarder.debug(
                num,
                "Possible data loss! Restarting connection!\n");
            forwarder.debug(
                num,
                e.what());

            auto dev_and_lock = getter.get_and_lock(num);
            dev_and_lock.first.cont.restart();
        }
    }
    catch (const Data_loss_except &e)
    {
        forwarder.debug(
            num,
            "Data loss!\n");

        if (close_on_data_loss)
        {
            forwarder.debug(
                num,
                "Shutting down connection!\n");
            forwarder.debug(
                num,
                e.what());

            remover.remove(num);
        }
        else
        {
            forwarder.debug(
                num,
                "Restarting connection!\n");
            forwarder.debug(
                num,
                e.what());

            auto dev_and_lock = getter.get_and_lock(num);
            dev_and_lock.first.cont.restart();
        }
    }
    catch (const std::exception &e)
    {
        forwarder.debug(
            num,
            "Unknown fatal exception!\n");
        forwarder.debug(
            num,
            e.what());

        throw;
    }
}

Logic::Exception_handler::Exception_handler(
    Notyfication_forwarder &&forwarder,
    Dev_remover &&remover,
    Dev_getter &&getter,
    bool close_on_timeout,
    bool close_on_data_loss,
    bool close_on_protocol_error)
    : Error_observer{
          [this](Error_job &job)
          {
              handle_exception(job.num, job.get_exc());
          }},
      forwarder{std::move(forwarder)}, remover{std::move(remover)}, getter{std::move(getter)}, close_on_timeout{close_on_timeout}, close_on_data_loss{close_on_data_loss}, close_on_protocol_error{close_on_protocol_error}
{
}