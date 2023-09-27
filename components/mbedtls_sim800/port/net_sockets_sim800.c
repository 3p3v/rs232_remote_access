#include <mbedtls/net_sockets.h>
#include <SIM_TCPIP.h>
#include <string.h>

extern SIM_intf *sim;

void mbedtls_net_init( mbedtls_net_context *ctx ) 
{
    ctx->fd = 0;
}

int mbedtls_net_recv( void *ctx, unsigned char *buf, size_t len )
{
    int err = SIM_TCP_read(sim, (SIM_con_num)((mbedtls_net_context *)ctx)->fd, buf, len);

    if (err == 0)
        err = -1;

    return err;
}

int mbedtls_net_send( void *ctx, const unsigned char *buf, size_t len )
{
    return SIM_TCP_write(sim, (SIM_con_num)((mbedtls_net_context *)ctx)->fd, buf, len);
}

// #ifdef MBEDTLS_DEFAULT_TCP_RESP_HANDLER
int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto )
{
    SIM_TCP_cmd cmd_tcp;
    SIM_cmd cmd;
    SIM_error err;
    SIM_listenTCP(&cmd_tcp, (SIM_con_num)ctx->fd, NULL);
    if ((err = SIM_run_multiple_launch(sim, &cmd_tcp)) != SIM_ok)
    {
        return err;
    }

    char protocol[4];
    if (proto == 0)
        strcpy(protocol, "TCP");
    else
        strcpy(protocol, "UDP");

    SIM_writeCIPSTART(&cmd, (SIM_con_num)ctx->fd, protocol, host, atoi(port));
    err = SIM_run(sim, &cmd);
    return err;
}
// #else
// int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto, void (*resp_handle)(int *))
// {
//     SIM_TCP_cmd cmd_tcp;
//     SIM_cmd cmd;
//     SIM_error err;
//     SIM_listenTCP(&cmd_tcp, (SIM_con_num)ctx->fd, resp_handle);
//     if ((err = SIM_run_multiple_launch(sim, &cmd_tcp)) != SIM_ok)
//     {
//         return err;
//     }

//     char protocol[4];
//     if (proto == 0)
//         strcpy(protocol, "TCP");
//     else
//         strcpy(protocol, "UDP");

//     SIM_writeCIPSTART(&cmd, (SIM_con_num)ctx->fd, protocol, host, atoi(port));
//     err = SIM_run(sim, &cmd);
//     return err;
// }
// #endif

void mbedtls_net_free( mbedtls_net_context *ctx )
{
    
}