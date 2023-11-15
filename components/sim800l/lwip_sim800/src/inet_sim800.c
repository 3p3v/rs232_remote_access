#include "inet_sim800.h"

/* ESP32 is little endian so pasted data has to be reverted */
uint16_t htons(uint16_t hostshort)
{
    uint16_t netshort = (hostshort << 8) | (hostshort >> 8);
    return netshort;
}

/* ESP32 is little endian so pasted data has to be reverted */
uint16_t ntohs(uint16_t netshort)
{
    uint16_t hostshort = (netshort << 8) | (netshort >> 8);
    return hostshort;
}