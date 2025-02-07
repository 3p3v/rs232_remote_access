#include <Notifier.hpp>

namespace Logic
{
    Notifier::Notifier(Notification_manager::Dev_num dev_num, const Notification_manager &manager)
        : dev_num{dev_num}, manager{manager}
    {
    }

    void Notifier::error(const std::exception &exc) const
    {
        manager.error(dev_num, exc);
    }

    void Notifier::debug(const std::string &info) const
    {
        manager.debug(dev_num, info);
    }
}