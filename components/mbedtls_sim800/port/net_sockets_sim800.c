#include <mbedtls/net_sockets.h>
#include <SIM_TCPIP.h>
#include <string.h>

void mbedtls_net_init(AT_socket *ctx, AT_socket_context* all_ctx)
{
    AT_socket_reinit(ctx, all_ctx);
}

int mbedtls_net_recv(AT_socket *ctx, unsigned char *buf, size_t len)
{
    int err = AT_socket_read(ctx->sim, ctx->fd, buf, len);

    if (err == AT_socket_err)
        return -1;
    else if (err == AT_socket_disconnected)
        return 0;

    return err;
}

int mbedtls_net_send(AT_socket *ctx, const unsigned char *buf, size_t len)
{
    return (int)AT_socket_write(ctx, buf, len);
}

void mbedtls_net_close(AT_socket *ctx)
{
    AT_socket_disconnect(ctx);
}

int mbedtls_net_connect(AT_socket *ctx, const char *host, const char *port, int proto)
{
    AT_socket_connect(ctx, host, port);
}

// void mbedtls_net_free(void *ctx)
// {
//     socket_context ctx = (socket_context *)ctx_;
//     SIM_cmd cmd;
//     SIM_cmd_init(&cmd);
//     SIM_error err;
//     if (ctx->sim->tcp_cmds[ctx->fd].cmd->resp.err == SIM_closed)
//     {
//         ctx->fd = -1;
//         return;
//     }

//     if ((err = SIM_run(ctx->sim, SIM_writeCIPCLOSE(&cmd, ctx->fd, 0))) == SIM_closedOk)
//     {
//         ctx->fd = -1;
//     }

//     SIM_cmd_free(&cmd);

//     return;
// }