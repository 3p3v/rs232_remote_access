#pragma once 

enum class Mqtt_error
{
    /* Unknown mqtt error */
    mqtt,
    /* Mqtt read error */
    mqtt_read,
    /* mqtt write error */
    mqtt_write
};