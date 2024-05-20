#include <Exception_handler.hpp>

void Logic::Exception_handler::handle_exception(Dev_num num, const std::exception &e)
{
    // TODO handle exception
}

Logic::Exception_handler::Exception_handler(Notyfication_forwarder &forwarder, Dev_remover &&controller)
    : Error_observer{
        [this](Error_job &job)
        { 
            handle_exception(job.num, job.exc); 
        }},
      forwarder{forwarder},
      controller{std::move(controller)}
{
}