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

int mbedtls_net_connect( mbedtls_net_context *ctx, const char *host, const char *port, int proto )
{
    // static unsigned char sockets[][2] = {{0, 0}, {1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}};
    // static SIM_cmd cmds[5];
    SIM_TCP_cmd cmd_tcp;
    SIM_cmd cmd;

    // for (unsigned int i = 0; i < SIM_con_5; i++)
    // {
    //     if (sockets[i][1] == 0)
    //     {
    //         ctx->fd = *sockets[i];
    //         cmd_tcp = &cmds[i];
    //         break;
    //     }
    // }

    // if (cmd_tcp == NULL)
    // {
    //     return SIM_err;
    // }
    
    SIM_error err;
    SIM_listenTCP(&cmd_tcp, (SIM_con_num)ctx->fd);
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

void mbedtls_net_free( mbedtls_net_context *ctx )
{
    
}