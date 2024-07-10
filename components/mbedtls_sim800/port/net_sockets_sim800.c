#include <mbedtls/net_sockets.h>
#include <SIM_TCPIP.h>
#include <string.h>

void mbedtls_net_init(void *ctx_, SIM_intf *sim)
{
    socket_context ctx = (socket_context *)ctx_;
    ctx->sim = sim;
    ctx->fd = -1;
}

int mbedtls_net_recv(void *ctx_, unsigned char *buf, size_t len)
{
    socket_context ctx = (socket_context *)ctx_;
    int err = SIM_TCP_read(ctx->sim, ctx->fd, buf, len);

    if (err == 0)
        err = -1;

    return err;
}

int mbedtls_net_send(void *ctx_, const unsigned char *buf, size_t len)
{
    socket_context ctx = (socket_context *)ctx_;
    return SIM_TCP_write(ctx->sim, ctx->fd, buf, len);
}

void mbedtls_net_close(void *ctx_)
{
    socket_context ctx = (socket_context *)ctx_;
    SIM_cmd cmd;
    SIM_cmd_init(&cmd);
    SIM_error err;
    if (ctx->sim->tcp_cmds[ctx->fd].cmd->resp.err == SIM_closed)
    {
        ctx->fd = -1;
        return;
    }

    if ((err = SIM_run(ctx->sim, SIM_writeCIPCLOSE(&cmd, ctx->fd, 1))) == SIM_closedOk)
    {
        ctx->fd = -1;
    }

    SIM_cmd_free(&cmd);

    SIM_listenTCP_free(ctx->sim, ctx->fd);

    return;
}

// #ifdef MBEDTLS_DEFAULT_TCP_RESP_HANDLER
int mbedtls_net_connect(void *ctx_, const char *host, const char *port, int proto)
{
    socket_context ctx = (socket_context *)ctx_;
    SIM_cmd cmd;
    SIM_cmd_init(&cmd);
    SIM_error err;

    if ((err = SIM_listenTCP(ctx->sim, ctx->fd, NULL)) != SIM_ok)
    {
        return err;
    }

    char protocol[4];
    if (proto == 0)
        strcpy(protocol, "TCP");
    else
        strcpy(protocol, "UDP");

    SIM_writeCIPSTART(&cmd, ctx->fd, protocol, host, atoi(port));
    err = SIM_run(ctx->sim, &cmd);

    SIM_cmd_free(&cmd);

    return err;
}

void mbedtls_net_free(void *ctx)
{
    socket_context ctx = (socket_context *)ctx_;
    SIM_cmd cmd;
    SIM_cmd_init(&cmd);
    SIM_error err;
    if (ctx->sim->tcp_cmds[ctx->fd].cmd->resp.err == SIM_closed)
    {
        ctx->fd = -1;
        return;
    }

    if ((err = SIM_run(ctx->sim, SIM_writeCIPCLOSE(&cmd, ctx->fd, 0))) == SIM_closedOk)
    {
        ctx->fd = -1;
    }

    SIM_cmd_free(&cmd);

    return;
}