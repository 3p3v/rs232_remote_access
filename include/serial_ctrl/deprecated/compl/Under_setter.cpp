#include <Under_setter.hpp>

namespace Logic
{
    void Under_setter::ack(char id)
    {
        helper->msgs.ack(id);
    }

    void Under_setter::ask_to_resend(char id)
    {
        mqtt.write_i(ack_s.data(), id);
    }

    Under_setter::Under_setter(Shared_helper_ptr &&helper)
        : helper{std::move(helper)}
    {
    }

    Under_setter::Under_setter(const Shared_helper_ptr &helper)
        : helper{helper}
    {
    }
}