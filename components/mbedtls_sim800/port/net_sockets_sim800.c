#include <mbedtls/net_sockets.h>
#include <SIM_TCPIP.h>

extern SIM_intf sim;

void mbedtls_net_init( mbedtls_net_context *ctx ) 
{
    ctx->fd = 0;
}

int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    return SIM_TCP_read(&sim, *(SIM_con_num *)ctx, buf, len);
}

int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    return SIM_TCP_write(&sim, *(SIM_con_num *)ctx, buf, len);
}
