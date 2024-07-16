#include "SIM_TCPIP.h"
#include <string.h>
#include <stdlib.h>

extern SIM_intf *sim; // TODO delete

extern const SIM_err_pair SIM_reservedResps[];

static SIM_error SIM_readCGATT_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    if (resp->err != SIM_ok)
        return resp->err;

    resp->err = SIM_retrieve(lines, "CGATT", resp);
    if (resp->err != SIM_ok)
        return resp->err;

    return resp->err;
}

SIM_cmd *SIM_readCGATT(SIM_cmd *cmd)
{
    SIM_param params[1];
    *params[0].name = '\0';
    SIM_setAT(cmd->at, "CGATT?", params);
    cmd->handlers[0] = &SIM_readCGATT_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_READCGATT_TIMEOUT;

    return cmd;
}

static SIM_error SIM_writeCSTT_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

SIM_cmd *SIM_writeCSTT(SIM_cmd *cmd, const char *apn, const char *username, const char *password)
{
    SIM_param params[4];
    strcpy(params[0].name, apn);
    if (username && password)
    {
        strcpy(params[1].name, username);
        strcpy(params[2].name, password);
        *params[3].name = '\0';
    }
    else
    {
        *params[1].name = '\0';
    }
    SIM_setAT(cmd->at, "CSTT", params);
    cmd->handlers[0] = &SIM_writeCSTT_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECSTT_TIMEOUT;

    return cmd;
}

static SIM_error SIM_execCIICR_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

SIM_cmd *SIM_execCIICR(SIM_cmd *cmd)
{
    SIM_param params[1];
    *params[0].name = '\0';
    SIM_setAT(cmd->at, "CIICR", params);
    cmd->handlers[0] = &SIM_execCIICR_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECCIICR_TIMEOUT;

    return cmd;
}

static SIM_errMsgEnd_pair SIM_execCIFSR_retrieveIP(SIM_line_pair *lines, SIM_resp *resp) // TODO
{
    SIM_errMsgEnd_pair err;

    SIM_line_pair *c_line = lines;
    for (;;)
    {
        if (c_line->ptr == NULL)
        {
            err.err = SIM_recErr;
            err.ptr = NULL;
            return err;
        }
        // else if (c_line)

        char *ip;
        for (unsigned int i = 0; i < 4; i++)
        {
            ip = (char *)SIM_util_strstr(c_line->ptr, c_line->len, ".");

            if (ip == NULL)
            {
                c_line++;
                break;
            }
            else
            {
                ip++;
                if (i < 3)
                {
                    continue;
                }
                else
                {
                    err.err = SIM_ok;
                    err.ptr = c_line->ptr + c_line->len;
                    return err;
                }
            }
        }
    }
}

static SIM_error SIM_execCIFSR_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    if (resp->err != SIM_noErrCode)
    {
        resp->msg_end = err.ptr;
        return resp->err;
    }

    err = SIM_execCIFSR_retrieveIP(lines, resp);
    resp->err = err.err;
    resp->msg_end = err.ptr;

    return err.err;
}

SIM_cmd *SIM_execCIFSR(SIM_cmd *cmd)
{
    SIM_param params[1];
    *params[0].name = '\0';
    SIM_setAT(cmd->at, "CIFSR", params);
    cmd->handlers[0] = &SIM_execCIFSR_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECCIICR_TIMEOUT;

    return cmd;
}

static SIM_error SIM_writeCIPSTART_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    if (err.err != SIM_ok && err.err != SIM_err)
        return SIM_noErrCode;

    SIM_err_pair c_st[] = {{.name = "., CONNECT OK\r\n", .err = SIM_connectOk},
                           {.name = "., ALREADY CONNECT\r\n", .err = SIM_alreadyConnect},
                           {.name = "., CONNECT FAIL\r\n", .err = SIM_connectFail},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err2 = SIM_retrieveCustomErr(lines, c_st);
    resp->err = err2.err;
    resp->msg_end = err2.ptr;
    if (resp->err == SIM_connectOk)
    {
        return SIM_ok;
    }
    else if (resp->err != SIM_alreadyConnect && resp->err != SIM_connectFail)
        return SIM_noErrCode;
    else
        return SIM_err;
    // todo check code and connect status
    // return err.err;
}

SIM_cmd *SIM_writeCIPSTART(SIM_cmd *cmd, const SIM_con_num n, char *mode, char *address, const unsigned int port)
{
    SIM_param params[5];
    unsigned char num = 0;
    if (n != SIM_con_def)
    {
        sprintf(params[0].name, "%i", (int)n);
        num++;
    }
    strcpy(params[0 + num].name, mode);
    strcpy(params[1 + num].name, address);
    sprintf(params[2 + num].name, "%u", (unsigned int)port);
    *params[3 + num].name = '\0';
    SIM_setAT(cmd->at, "CIPSTART", params);
    cmd->handlers[0] = &SIM_writeCIPSTART_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPSTART_TIMEOUT;

    return cmd;
}

static SIM_error SIM_execCIPSEND_handler2(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_err_pair c_st[] = {{.name = "SEND OK\r\n", .err = SIM_sendOk},
                           {.name = "SEND FAIL\r\n", .err = SIM_sendFail},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);
    resp->err = err.err;
    resp->msg_end = err.ptr;

    if (err.err == SIM_sendOk)
        err.err = SIM_ok;
    else if (err.err == SIM_sendFail)
        err.err = SIM_err;
    else
        err.err = SIM_noErrCode;

    return err.err;
}

static SIM_error SIM_execCIPSEND_handler1(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_err_pair c_st[] = {{.name = "> ", .err = SIM_ok},
                           {.name = "ERROR\r\n", .err = SIM_err},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);
    resp->err = err.err;
    resp->msg_end = err.ptr;

    if (err.err == SIM_ok)
        err.err = LL_SIM_sendData((SIM_intf *)sim, resp->send_data, resp->send_data_len);

    return err.err;
}

SIM_cmd *SIM_execCIPSEND(SIM_cmd *cmd, void *send_data, SIM_data_len send_data_len)
{
    SIM_param params[1];
    *params[0].name = '\0';
    SIM_setAT(cmd->at, "CIPSEND", params);
    cmd->handlers[0] = &SIM_execCIPSEND_handler2;
    cmd->handlers[1] = &SIM_execCIPSEND_handler1;
    cmd->handlers_num = 2;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->resp.send_data = send_data;
    cmd->resp.send_data_len = send_data_len;
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECCIPSEND_TIMEOUT;

    return cmd;
}

static SIM_error SIM_writeCIPSEND_handler2(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    static int times_sent = 0;
    times_sent++;
    if (times_sent == 5 || times_sent == 4)
    {
        printf("%s\r\n", lines_beg->ptr);
    }

    if (times_sent == 5)
        printf("SEND\r\n");

    SIM_err_pair c_st[] = {{.name = "OK\r\n", .err = SIM_sendOk},
                           {.name = "., SEND OK\r\n", .err = SIM_sendOk},
                           {.name = "., SEND FAIL\r\n", .err = SIM_sendFail},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);

    resp->err = err.err;
    resp->msg_end = err.ptr;

    if (err.err == SIM_noErrCode || *err.ptr_beg != resp->at[11])
        return SIM_noErrCode;
    if (err.err == SIM_sendOk)
        return SIM_ok;
    else if (err.err == SIM_sendFail)
        return SIM_err;
    else
        return SIM_noErrCode;
}

static SIM_error SIM_writeCIPSEND_handler1(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_err_pair c_st[] = {{.name = "> ", .err = SIM_ok},
                           {.name = "ERROR\r\n", .err = SIM_err},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);
    resp->err = err.err;
    resp->msg_end = err.ptr;

    if (err.err == SIM_ok)
        err.err = LL_SIM_sendData((SIM_intf *)sim, resp->send_data, resp->send_data_len);

    return err.err;
}

SIM_cmd *SIM_writeCIPSEND(SIM_cmd *cmd, SIM_con_num n, SIM_data_len length, void *send_data, SIM_data_len send_data_len)
{
    SIM_param params[4];
    if (n > SIM_con_5 || n < SIM_con_def)
    {
        return NULL;
    }
    else if (n != SIM_con_def)
    {
        if (length != 0)
        {
            sprintf(params[0].name, "%u", (unsigned int)n);
            sprintf(params[1].name, "%u", length);
            *params[2].name = '\0';
        }
        else
        {
            sprintf(params[0].name, "%u", (unsigned int)n);
            *params[1].name = '\0';
        }
    }
    else if (n == SIM_con_def)
    {
        if (length == 0)
            return NULL;

        sprintf(params[0].name, "%u", length);
        *params[1].name = '\0';
    }
    SIM_setAT(cmd->at, "CIPSEND", params);
    cmd->handlers[0] = &SIM_writeCIPSEND_handler2;
    cmd->handlers[1] = &SIM_writeCIPSEND_handler1;
    cmd->handlers_num = 2;
    SIM_respNULL(&cmd->resp, cmd->at);
    static SIM_err_pair c_st[] = {{.name = "> ", .err = SIM_ok},
                                  {.name = "ERROR\r\n", .err = SIM_err},
                                  {.name = "OK\r\n", .err = SIM_sendOk},
                                  {.name = "., SEND OK\r\n", .err = SIM_sendOk},
                                  {.name = "., SEND FAIL\r\n", .err = SIM_sendFail},
                                  {.name = NULL, .err = SIM_noErrCode}};
    cmd->resp.pos_resps = c_st;
    cmd->resp.send_data = send_data;
    cmd->resp.send_data_len = send_data_len;
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPSEND_TIMEOUT;

    return cmd;
}

static SIM_error SIM_listenTCP_cipmux0_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    resp->data = NULL;
    resp->data_len = 0;
    resp->params_num = 0;
    resp->resp_name_len = 0;

    SIM_err_pair c_st[] = {{.name = "CLOSED\r\n", .err = SIM_closed},
                           {.name = NULL, .err = SIM_receive}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);

    if (err.err == SIM_closed)
    {
        // SIM_listenTCP_closed_handler(/* EDIT */);
        resp->err = err.err;
        resp->msg_end = err.ptr;
        return SIM_err;
    }
    else
    {
        // SIM_listenTCP_receive_handler(/* EDIT */);
        // resp->data = buf;
        resp->err = err.err;
        err = SIM_retrieveCustomErr(lines, SIM_reservedResps);
        resp->data_len = err.ptr - (char *)resp->data;
        resp->msg_end = err.ptr;

        return SIM_ok;
    }
}

static SIM_error SIM_listenTCP_cipmux1_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    SIM_err_pair c_st[] = {{.name = "+RECEIVE,", .err = SIM_receive},
                           {.name = "., CLOSED\r\n", .err = SIM_closed},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);

    // check if data was received from the TCP connection
    if (err.err == SIM_receive)
    {
        char *resp_end = strstr(err.ptr_beg, ":\r\n");
        if (resp_end == NULL)
        {
            // message distorted?
            return SIM_err;
        }

        // if tcp_ret == NULL, that means the semaphore was already taken
        if (((SIM_intf *)sim)->tcp_ret == NULL)
            xSemaphoreTake(resp->data_mutex, portMAX_DELAY);
        // save response to buffer
        resp->resp = realloc(resp->resp, (sizeof(unsigned char) * (resp_end - err.ptr_beg) - strlen("+RECEIVE,")));
        resp->resp_len = (resp_end - err.ptr_beg) - strlen("+RECEIVE,");
        memcpy(resp->resp, err.ptr_beg + strlen("+RECEIVE,"), resp->resp_len);

        // get parameters
        resp->params[0].ptr = resp->resp;
        resp->params[0].len = 1;
        resp->params[1].ptr = resp->resp + 2;
        resp->params[1].len = resp->resp_len - (resp->params[1].ptr - resp->resp);
        resp->params_num = 2;

        // check if got same amount of data as response says
        char data_len_str[10 /* at least resp->params[1].len + 1 */] = {};
        memcpy(data_len_str, resp->params[1].ptr, resp->params[1].len);
        int data_len = atoi(data_len_str);
        if (((SIM_intf *)sim)->rec_len - (err.ptr - (char *)((SIM_intf *)sim)->buf) < data_len)
        {
            // wait for more data to come
            return sim_msgDetect;
        }

        // get data
        resp->msg_end = (lines + (err.line_num + 1))->ptr + data_len;

        if (resp->data_len != 0)
        {
            resp->data = realloc(resp->data, sizeof(unsigned char) * (resp->data_len + (char *)resp->msg_end - err.ptr));
            memcpy(resp->data + resp->data_len, err.ptr, (char *)resp->msg_end - err.ptr);
        }
        else
        {
            resp->data = malloc(sizeof(unsigned char) * ((char *)resp->msg_end - err.ptr));
            memcpy(resp->data, err.ptr, (char *)resp->msg_end - err.ptr);
        }
        resp->data_len += (char *)resp->msg_end - err.ptr;

        xSemaphoreGive(resp->data_mutex);

        resp->err = SIM_receive;
        return SIM_ok;
    }
    else if (err.err == SIM_closed)
    {
        // SIM_listenTCP_closed_handler(/* EDIT */);
        resp->err = err.err;
        resp->msg_end = err.ptr;

        return SIM_err;
    }
    else
    {
        // response from SIM doesn't match any code
        return SIM_noErrCode;
    }
}

static SIM_error SIM_writeCIPCLOSE_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_errMsgEnd_pair err2 = SIM_retrieveCustomErr(lines, resp->pos_resps);
    resp->err = err2.err;
    resp->msg_end = err2.ptr;
    if (resp->err == SIM_noErrCode)
    {
        return SIM_noErrCode;
    }
    else if (resp->err == SIM_closedOk)
    {
        return SIM_ok;
    }
    else
    {
        return SIM_err;
    }
}

SIM_cmd *SIM_writeCIPCLOSE(SIM_cmd *cmd, const SIM_con_num id, const char n)
{
    SIM_param params[3];
    unsigned char num = 0;
    if (id == SIM_con_def)
    {
        sprintf(params[num++].name, "%u", (unsigned int)id);
    }
    sprintf(params[num++].name, "%u", (unsigned int)n);
    *params[num++].name = '\0';
    SIM_setAT(cmd->at, "CIPCLOSE", params);
    cmd->handlers[0] = &SIM_writeCIPCLOSE_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    if (id == SIM_con_def)
    {
        static SIM_err_pair c_st[] = {{.name = "ERROR\r\n", .err = SIM_err},
                                      {.name = "CLOSE OK\r\n", .err = SIM_closedOk},
                                      {.name = NULL, .err = SIM_noErrCode}};
        cmd->resp.pos_resps = c_st;
    }
    else
    {
        static SIM_err_pair c_st[] = {{.name = "ERROR\r\n", .err = SIM_err},
                                      {.name = "., CLOSE OK\r\n", .err = SIM_closedOk},
                                      {.name = NULL, .err = SIM_noErrCode}};
        cmd->resp.pos_resps = c_st;
    }
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPCLOSE_TIMEOUT;

    return cmd;
}

SIM_error SIM_listenTCP(SIM_intf *sim, const SIM_con_num n, void (*resp_handler)(SIM_error *))
{
    SIM_error err;
    
    if (n > SIM_con_5 || n < SIM_con_def)
        return SIM_err;

    SIM_TCP_cmd **cmd = &sim->tcp_cmds[n].cmd;

    /* Check if the stream is not already used */
    xSemaphoreTake(sim->exec_mutex, portMAX_DELAY);
    if (*cmd == NULL)
    {
        /* OK */
        /* Allocate stream */
        *cmd = malloc(sizeof(SIM_TCP_cmd));
        /* Set handlers */
        if (n == SIM_con_def)
            (*cmd)->handler = &SIM_listenTCP_cipmux0_handler;
        else
            (*cmd)->handler = &SIM_listenTCP_cipmux1_handler;
        if (resp_handler == NULL)
            (*cmd)->resp_handler = &SIM_listenTCP_resp_handler;
        else
            (*cmd)->resp_handler = resp_handler;
        /* NULL resp */
        memset(&(*cmd)->resp, 0, sizeof((*cmd)->resp));
        (*cmd)->resp.err = SIM_unknown;
        (*cmd)->resp.data_mutex = xSemaphoreCreateBinary();
        xSemaphoreGive((*cmd)->resp.data_mutex);

        err = SIM_ok;
    }
    else
    {
        /* Straem was already allocated, return error */
        err = SIM_err;
    }
    xSemaphoreGive(sim->exec_mutex);

    return err;
}

SIM_error SIM_listenTCP_free(SIM_intf *sim, const SIM_con_num n)
{
    SIM_error err;
    
    if (n > SIM_con_5 || n < SIM_con_def)
        return SIM_err;

    SIM_TCP_cmd **cmd = &sim->tcp_cmds[n].cmd;
    
    /* Check if the stream was inited before */
    xSemaphoreTake(sim->exec_mutex, portMAX_DELAY);
    if (*cmd != NULL)
    {
        /* OK */
        /* Free the stream */
        SIM_respNULL(&(*cmd)->resp, NULL);
        vSemaphoreDelete((*cmd)->resp.data_mutex);
        free(*cmd);

        err = SIM_ok;
    }
    else
    {
        /* Trying to free unused stream! */   
        err = SIM_err;
    }
    xSemaphoreGive(sim->exec_mutex);

    return err;
}

SIM_error SIM_listenTCP_setHandler(SIM_intf *sim, const SIM_con_num n, void (*resp_handler)(SIM_error *))
{
    if (n > SIM_con_5 || n < SIM_con_0)
        return SIM_err;

    SIM_error err = SIM_noErrCode;
    SIM_TCP_cmd_grip *cmd_grip;
    SIM_TCP_cmd *cmd;

    // Find the right stream
    cmd_grip = &sim->tcp_cmds[n];
    cmd = sim->tcp_cmds[n].cmd;
    if (!cmd)
        return SIM_err;

    cmd->resp_handler = resp_handler;

    return SIM_ok;
}

static SIM_error SIM_writeCIPMUX_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

SIM_cmd *SIM_writeCIPMUX(SIM_cmd *cmd, const unsigned char n)
{
    SIM_param params[2];
    sprintf(params[0].name, "%u", (unsigned int)n);
    *params[1].name = '\0';
    SIM_setAT(cmd->at, "CIPMUX", params);
    cmd->handlers[0] = &SIM_writeCIPMUX_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPMUX_TIMEOUT;

    return cmd;
}