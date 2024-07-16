#pragma once

#include <LL_SIM.h>

#define AT_SOCKET_READ_TIMEOUT 5000

typedef SIM_con_num AT_con_num;

typedef enum
{
    AT_socket_ok = 0,
    AT_socket_err = -99,
    AT_socket_disconnected
} AT_socket_code;

typedef enum
{
    AT_socket_state_uninited = 0,
    AT_socket_state_inited,
    AT_socket_state_connected,
    AT_socket_state_disconnected
} AT_socket_state_code;

typedef struct AT_socket_info
{
    SIM_TCP_cmd_grip *cmd;
    AT_socket_state_code state;
} AT_socket_info;

typedef struct AT_socket_context
{
    AT_socket_info sockets[LL_SIM_DEF_TCP_CMDS_NUM];
    SIM_intf *sim;
} AT_socket_context;

typedef struct AT_socket
{
    SIM_con_num fd;
    AT_socket_context *ctx;
} AT_socket;

void AT_socket_reinit(AT_socket* socket, AT_socket_context* ctx);

AT_socket AT_socket_init(AT_socket_context* ctx);

void AT_socket_context_reinit(AT_socket_context *ctx, SIM_intf *sim);

AT_socket_context AT_socket_context_init(SIM_intf *sim);

AT_socket_code AT_socket_connect(AT_socket* socket, const char *host, const char *port);

int AT_socket_read(AT_socket* socket, void *buf, unsigned int len);

int AT_socket_write(AT_socket* socket, void *buf, unsigned int len);

AT_socket_code AT_socket_disconnect(AT_socket* socket);

unsigned int AT_socket_buf_check(AT_socket *socket);