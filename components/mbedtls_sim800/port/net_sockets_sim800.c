#include <mbedtls/net_sockets.h>
#include <AT_socket.h>
#include <string.h>

void mbedtls_net_init_(AT_socket *ctx, AT_socket_context* all_ctx)
{
    AT_socket_reinit(ctx, all_ctx);
}

int mbedtls_net_recv(void *ctx_, unsigned char *buf, size_t len)
{
    AT_socket *ctx = (AT_socket *)ctx_;
    
    int err = AT_socket_read(ctx, buf, len);

    if (err == AT_socket_err)
        return -1;
    else if (err == AT_socket_disconnected)
        return 0;

    return err;
}

int mbedtls_net_send(void *ctx_, const unsigned char *buf, size_t len)
{
    AT_socket *ctx = (AT_socket *)ctx_;
    
    return (int)AT_socket_write(ctx, buf, len);
}

void mbedtls_net_close_(AT_socket *ctx)
{   
    AT_socket_disconnect(ctx);
}

int mbedtls_net_connect_(AT_socket *ctx, const char *host, const char *port)
{
    return AT_socket_connect(ctx, host, port);
}

void mbedtls_net_free_(AT_socket *ctx)
{
    AT_socket_disconnect(ctx);
}