#include <Mqtt_msg_cont.hpp>
#include <algorithm>
#include <stdexcept>

namespace Ip_serial
{

    Mqtt_msg &Mqtt_msg_cont::operator[](unsigned char id)
    {
        auto msg = std::find_if(msgs.begin(),
                                msgs.end(),
                                [this, id](auto &&c)
                                {
                                    if (c.id == id && c.used == false)
                                        return true;
                                    else
                                        return false;
                                });

        if (msg == msgs.end())
            throw std::logic_error{"This message does not exist!"};

        msg->used = true;
        return *msg;
    }

    Mqtt_msg &Mqtt_msg_cont::first_free()
    {
        auto msg = std::find_if(msgs.begin(),
                                msgs.end(),
                                [this](auto &&c)
                                {
                                    return static_cast<bool>(c.freed);
                                });

        if (msg == msgs.end())
            throw std::logic_error{"No more free msgs!"};

        msg->freed = false;
        msg->used = true;

        return *msg;
    }

    Mqtt_msg &Mqtt_msg_cont::oldest()
    {
        decltype(msgs)::iterator msg = msgs.begin();
        decltype(msgs)::iterator iter = msgs.begin();

        std::for_each(msgs.begin(),
                      msgs.end(),
                      [this, &msg, &iter](auto &&c)
                      {
                          if (c.id < msg->id && c.used == false)
                          {
                            msg = iter;
                          }

                          iter++;
                      });

        msg->freed = false;
        msg->used = true;

        return *msg;
    }

    void Mqtt_msg_cont::free_untill(unsigned char id)
    {
        if ((id - max_saved) >= min_msg_num)
        {
            std::for_each(msgs.begin(),
                          msgs.end(),
                          [this, id](auto &&c)
                          {
                              if (c.id < id && c.used == false)
                              {
                                  c.freed = true;
                              }
                          });
        }
        else
        {
            auto offset = max_msg_num - (id % min_msg_num);

            std::for_each(msgs.begin(),
                          msgs.end(),
                          [this, id, offset](auto &&c)
                          {
                              if (c.id > offset || c.id < id)
                              {
                                  c.freed = true;
                              }
                          });
        }
    }

    void Mqtt_msg_cont::unmark(unsigned char id)
    {
        operator[](id).used = false;
    }
}