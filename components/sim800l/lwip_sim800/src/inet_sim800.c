#include "inet_sim800.h"

/* ESP32 is little endian so pasted data has to be reverted */
uint16_t htons(uint16_t hostshort)
{
    void *ptr = &hostshort;
    uint8_t temp = *(uint8_t *)ptr;
    *((uint16_t *)ptr) <<= 8;
    *((uint8_t *)ptr + 1) = temp;
    return hostshort;
}

/* ESP32 is little endian so pasted data has to be reverted */
uint16_t ntohs(uint16_t netshort)
{
    void *ptr = &netshort;
    uint8_t temp = *(uint8_t *)ptr;
    *((uint16_t *)ptr) <<= 8;
    *((uint8_t *)ptr + 1) = temp;
    return netshort;
}