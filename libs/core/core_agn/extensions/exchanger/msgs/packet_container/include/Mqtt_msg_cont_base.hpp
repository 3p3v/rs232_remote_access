#pragma once

class Mqtt_msg_cont_base
{
public:
    Mqtt_msg_cont_base() = default;
    Mqtt_msg_cont_base(const Mqtt_msg_cont_base &) = delete;
    Mqtt_msg_cont_base(Mqtt_msg_cont_base &&) = default;
    Mqtt_msg_cont_base &operator=(const Mqtt_msg_cont_base &) = delete;
    Mqtt_msg_cont_base &operator=(Mqtt_msg_cont_base &&) = default;
    virtual ~Mqtt_msg_cont_base() = 0;
};
