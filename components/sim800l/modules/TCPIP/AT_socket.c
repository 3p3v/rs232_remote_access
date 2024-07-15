#include <AT_socket.h>

void AT_socket_reinit(AT_socket* socket, AT_socket_context* ctx)
{
    socket->ctx = ctx;
    socket->fd = -1;
}

AT_socket AT_socket_init(AT_socket_context* ctx)
{
    AT_socket socket = {
        .ctx = ctx,
        .fd = -1;
    }
    
    return socket;
}

static void AT_socket_info_init(AT_socket_info *info, SIM_TCP_cmd_grip *cmd)
{
    info->cmd = cmd;
    info->state = AT_socket_state_uninited;
}

void AT_socket_context_reinit(AT_socket_context *ctx, SIM_intf *sim)
{
    ctx->sim = sim;
    
    for (unsigned char i = 0; i < sizeof(ctx->sockets); i++)
    {
        AT_socket_info_init(ctx->sockets + i, (sim->tcp_cmds + i)->cmd); // FIXME for now sim lib uses same socket id as array iterator number, fix when changed 
    }
}

AT_socket_context AT_socket_context_init(SIM_intf *sim)
{
    AT_socket_context ctx;
    
    AT_socket_context_reinit(&ctx);

    return ctx;
}

static AT_socket_code get_free_socket(AT_socket* socket)
{   
    AT_socket_context *ctx = socket->ctx;
    
    for (SIM_con_num i = SIM_con_0; i <= SIM_con_5; i++)
    {
        AT_socket_info *socket_ = (ctx->sockets + i);
        
        if (socket_->state == AT_socket_state_uninited)
        {
            socket_->state = AT_socket_state_inited;
            socket->fd = i;

            return AT_socket_ok;
        }
    }

    return AT_socket_err;
}

static void socket_set_state(AT_socket* socket, AT_socket_state_code state)
{
    (socket->ctx->sockets + socket->fd)->state = state;
}

AT_socket_code AT_socket_connect(AT_socket* socket, const char *host, const char *port)
{
    if (get_free_socket(socket))
    {
        return AT_socket_err;
    }

    SIM_cmd cmd;
    SIM_cmd_init(&cmd);
    SIM_error err;

    if ((err = SIM_listenTCP(socket->ctx->sim, socket->fd, NULL)) != SIM_ok)
    {
        return AT_socket_err;
    }

    SIM_writeCIPSTART(&cmd, socket->fd, "TCP", host, atoi(port));
    err = SIM_run(socket->ctx->sim, &cmd);

    SIM_cmd_free(&cmd);

    if (err != SIM_ok)
    {
        return AT_socket_err; 
    }
    else
    {
        socket_set_state(socket, AT_socket_state_connected);
        
        return AT_socket_ok
    }
}

static bool socket_disconnected(SIM_error err)
{
    if (err == SIM_closed || err == SIM_closedOk)
        return true;
    else
        return false;
}

int AT_socket_read(AT_socket* socket, void *buf, unsigned int len)
{
    SIM_con_num n = socket->fd;
    // AT_socket_code err = AT_socket_ok;
    SIM_TCP_cmd_grip *cmd_grip;
    SIM_TCP_cmd *cmd;
    SIM_resp *resp;

    // Find the right stream
    cmd_grip = socket->ctx->sim->tcp_cmds + n;
    cmd = cmd_grip->cmd;
    if (cmd)
        resp = cmd->resp;
    else
        return AT_socket_err;

    SIM_resp err = resp->err;
    // Check if socket disconnected
    if (socket_disconnected(err))
    {
        socket_set_state(socket, AT_socket_state_disconnected);
        return AT_socket_disconnected;
    }

    // no data to receive, wait for it
    err = SIM_noErrCode;
    if (resp->data_len == 0)
    {
        xQueueReceive(cmd_grip->queue, &err, AT_SOCKET_READ_TIMEOUT / portTICK_PERIOD_MS);
    }

    if (socket_disconnected(err))
    {
        socket_set_state(socket, AT_socket_state_disconnected);
        return AT_socket_disconnected;
    }

    // Read the data and delete it from the buffer
    xSemaphoreTake(resp->data_mutex, portMAX_DELAY);
    unsigned int read_len;

    START:
    if (len < resp->data_len)
    {
        memcpy(buf, resp->data, len);
        memcpy(resp->data, resp->data + len, resp->data_len - len);
        read_len = len;
        resp->data = realloc(resp->data, (resp->data_len = (resp->data_len - len)));
        goto EXIT;
    }
    else if (len == resp->data_len)
    {
        memcpy(buf, resp->data, len);
        free(resp->data);
        read_len = len;
        resp->data_len = 0;
        goto EXIT;
    }
    else
    {
        xSemaphoreGive(resp->data_mutex);
        // wait for enough data while raw data is still being processed by main task
        // TODO delete
        while ((len > resp->data_len) && sim->rec_len)
        {
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }
        xSemaphoreTake(resp->data_mutex, portMAX_DELAY);

        if (socket_disconnected(err))
        {
            socket_set_state(socket, AT_socket_state_disconnected);
            read_len = (int)AT_socket_disconnected;
            goto EXIT;
        }

        if (!resp->data_len)
        {
            /* No data received */
            read_len = 0;
            goto EXIT;
        }
        else if (len > resp->data_len)
        {
            /* Too little data was received */
            memcpy(buf, resp->data, resp->data_len);
            free(resp->data);
            read_len = resp->data_len;
            resp->data_len = 0;
            goto EXIT;
        }
        else
        {
            goto START;
        }
    }

EXIT:
    xSemaphoreGive(resp->data_mutex);
    return read_len;
}

int AT_socket_write(AT_socket* socket, void *buf, unsigned int len)
{
    SIM_error err;
    SIM_cmd cmd;
    SIM_cmd_init(&cmd);

    if ((err = SIM_run(sim, SIM_writeCIPSEND(&cmd, socket->fd, len, buf, len))) != SIM_ok)
        return (int)AT_socket_err;

    return len;
}

AT_socket_code AT_socket_disconnect(AT_socket* socket)
{
    SIM_cmd cmd;
    SIM_cmd_init(&cmd);
    SIM_error err;

    if (socket->ctx->sim->tcp_cmds[ctx->fd].cmd->resp.err == SIM_closed)
    {
        socket_set_state(socket, AT_socket_state_disconnected);
        return AT_socket_ok;
    }

    err = SIM_run(socket->ctx->sim, SIM_writeCIPCLOSE(&cmd, socket->fd, 1));

    SIM_cmd_free(&cmd);

    SIM_listenTCP_free(socket->ctx->sim, socket->fd);

    if (err == SIM_ok)
    {
        socket_set_state(socket, AT_socket_state_disconnected);
        return AT_socket_ok;
    }
    else 
    {
        return AT_socket_err;
    }
}