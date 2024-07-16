#pragma once 

struct AT_socket_context;
typedef struct AT_socket_context AT_socket_context;

struct AT_socket;
typedef struct AT_socket AT_socket;

void mbedtls_net_init_(AT_socket *ctx_, AT_socket_context* all_ctx);
void mbedtls_net_close_(AT_socket *ctx_);
int mbedtls_net_connect_(AT_socket *ctx, const char *host, const char *port);
void mbedtls_net_free_(AT_socket *ctx);