#include "SIM_TCPIP.h"
#include <string.h>

extern const SIM_err_pair SIM_reservedResps[];

static SIM_error SIM_readCGATT_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

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
    *params[0].name = NULL;
    SIM_setAT(cmd->at, "CGATT?", params);
    cmd->handlers[0] = &SIM_readCGATT_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_READCGATT_TIMEOUT;

    return cmd;
}

static SIM_error SIM_writeCSTT_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

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
        *params[3].name = NULL;
    }
    else
    {
        *params[1].name = NULL;
    }
    SIM_setAT(cmd->at, "CSTT", params);
    cmd->handlers[0] = &SIM_writeCSTT_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECSTT_TIMEOUT;

    return cmd;
}

static SIM_error SIM_execCIICR_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

SIM_cmd *SIM_execCIICR(SIM_cmd *cmd)
{
    SIM_param params[1];
    *params[0].name = NULL;
    SIM_setAT(cmd->at, "CIICR", params);
    cmd->handlers[0] = &SIM_execCIICR_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECCIICR_TIMEOUT;

    return cmd;
}

static SIM_errMsgEnd_pair SIM_execCIFSR_retrieveIP(char *buf, unsigned int rec_len, SIM_resp *resp) // TODO
{
    SIM_errMsgEnd_pair err;
    resp->data = buf + strlen("AT+CIFSR\r\r\n");

    if (resp->data + resp->data_len > buf + rec_len)
    {
        resp->data = NULL;
        resp->data_len = 0;
        err.err = SIM_recErr;
        err.ptr = NULL;
        return err;
    }
    else
    {
        const char *end = strstr(resp->data, "\r\n");
        if (end != NULL)
        {
            resp->data_len = end - (char *)resp->data;
            err.err = SIM_ok;
            err.ptr = end + strlen("\r\n");
            return err;
        }
        else
        {
            resp->data = NULL;
            resp->data_len = 0;
            err.err = SIM_recErr;
            err.ptr = NULL;
            return err;
        }
    }
}

static SIM_error SIM_execCIFSR_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    if (resp->err != SIM_noErrCode)
    {
        resp->msg_end = err.ptr;
        return resp->err;
    }

    err = SIM_execCIFSR_retrieveIP(buf, rec_len, resp);
    resp->err = err.err;
    resp->msg_end = err.ptr;

    return err.err;
}

SIM_cmd *SIM_execCIFSR(SIM_cmd *cmd)
{
    SIM_param params[1];
    *params[0].name = NULL;
    SIM_setAT(cmd->at, "CIFSR", params);
    cmd->handlers[0] = &SIM_execCIFSR_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECCIICR_TIMEOUT;

    return cmd;
}

static SIM_error SIM_writeCIPSTART_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

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
    if (resp->err != SIM_connectOk && resp->err != SIM_alreadyConnect && resp->err != SIM_connectFail)
        return SIM_noErrCode;
    else if (resp->err == SIM_connectOk)
        return SIM_ok;
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
    *params[3 + num].name = NULL;
    SIM_setAT(cmd->at, "CIPSTART", params);
    cmd->handlers[0] = &SIM_writeCIPSTART_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPSTART_TIMEOUT;

    return cmd;
}

static SIM_error SIM_execCIPSEND_handler2(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

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

static SIM_error SIM_execCIPSEND_handler1(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

    SIM_err_pair c_st[] = {{.name = "> ", .err = SIM_ok},
                           {.name = "ERROR\r\n", .err = SIM_err},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);
    resp->err = err.err;
    resp->msg_end = err.ptr;

    if (err.err == SIM_ok)
        err.err = LL_SIM_sendData((SIM_intf *)sim, resp->send_data, resp->send_data_len);

    if (err.err == SIM_ok)
        err.err = LL_SIM_sendData((SIM_intf *)sim, "\032", strlen("\032"));

    return err.err;
}

SIM_cmd *SIM_execCIPSEND(SIM_cmd *cmd, void *send_data, SIM_data_len send_data_len)
{
    SIM_param params[1];
    *params[0].name = NULL;
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

static SIM_error SIM_writeCIPSEND_handler2(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

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

static SIM_error SIM_writeCIPSEND_handler1(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

    SIM_err_pair c_st[] = {{.name = "> ", .err = SIM_ok},
                           {.name = "ERROR\r\n", .err = SIM_err},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);
    resp->err = err.err;
    resp->msg_end = err.ptr;

    if (err.err == SIM_ok)
        err.err = LL_SIM_sendData((SIM_intf *)sim, resp->send_data, resp->send_data_len);

    if (err.err == SIM_ok)
        err.err = LL_SIM_sendData((SIM_intf *)sim, "\032", strlen("\032"));

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
            *params[2].name = NULL;
        }
        else
        {
            sprintf(params[0].name, "%u", (unsigned int)n);
            *params[1].name = NULL;
        }
    }
    else if (n == SIM_con_def)
    {
        if (length == 0)
            return NULL;

        sprintf(params[0].name, "%u", length);
        *params[1].name = NULL;
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
    cmd->resp.pos_resps = &c_st;
    cmd->resp.send_data = send_data;
    cmd->resp.send_data_len = send_data_len;
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPSEND_TIMEOUT;

    return cmd;
}

// static void SIM_listenTCP_receive_handler(/* EDIT */)
// {
//     /* EDIT */

//     /********/
// }

// static void SIM_listenTCP_closed_handler(/* EDIT */)
// {
//     /* EDIT */

//     /********/
// }

static SIM_error SIM_listenTCP_cipmux0_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    resp->data = NULL;
    resp->data_len = 0;
    resp->params_num = 0;
    resp->resp_name_len = 0;

    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

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
        resp->data = buf;
        resp->err = err.err;
        err = SIM_retrieveCustomErr(lines, SIM_reservedResps);
        resp->data_len = err.ptr - (char *)resp->data;
        resp->msg_end = err.ptr;

        return SIM_ok;
    }
}

static SIM_error SIM_listenTCP_cipmux1_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    // resp->data = NULL;
    // resp->data_len = 0;
    // resp->params_num = 0;
    // resp->resp_name_len = 0;

    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_data_len limes_num = SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

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
        if (rec_len - (err.ptr - buf) < data_len)
        {
            // wait for more data to come
            return sim_msgDetect;
        }

        // get data
        SIM_line_pair *lines_ptr = (lines + (err.line_num + 1));
        SIM_errMsgEnd_pair err2 = SIM_retrieveCustomErr(lines_ptr, SIM_reservedResps);
        void * end_ptr;
        if (err2.err != SIM_noErrCode)
        {
            end_ptr = err2.ptr_beg - strlen ("\r\n");
        }
        else
        {
            end_ptr = err2.ptr;

        }

        resp->msg_end = end_ptr;
        if (resp->data_len != 0)
        {
            resp->data = realloc(resp->data, sizeof(unsigned char) * (resp->data_len + (char *)end_ptr - err.ptr));
            memcpy(resp->data + resp->data_len, err.ptr, (char *)end_ptr - err.ptr);
        }
        else
        {
            resp->data = malloc(sizeof(unsigned char) * ((char *)end_ptr - err.ptr));
            memcpy(resp->data, err.ptr, (char *)end_ptr - err.ptr);
        }
        resp->data_len += (char *)end_ptr - err.ptr;

        xSemaphoreGive(resp->data_mutex);

        // SIM_listenTCP_receive_handler(/* EDIT */);
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

static SIM_error SIM_writeCIPCLOSE_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

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
    if (resp->err != SIM_connectOk && resp->err != SIM_alreadyConnect && resp->err != SIM_connectFail)
        return SIM_noErrCode;
    else if (resp->err == SIM_connectOk)
        return SIM_ok;
    else
        return SIM_err;
    // todo check code and connect status
    // return err.err;
}

SIM_cmd *SIM_writeCIPCLOSE(SIM_cmd *cmd, const SIM_con_num id, const char n)
{
    SIM_param params[3];
    sprintf(params[0].name, "%u", (unsigned int)id);
    sprintf(params[1].name, "%u", (unsigned int)n);
    *params[2].name = NULL;
    SIM_setAT(cmd->at, "CIPCLOSE", params);
    cmd->handlers[0] = &SIM_writeCIPCLOSE_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    static SIM_err_pair c_st[] = {{.name = "> ", .err = SIM_ok},
                           {.name = "ERROR\r\n", .err = SIM_err},
                           {.name = "OK\r\n", .err = SIM_sendOk},
                           {.name = "., SEND OK\r\n", .err = SIM_sendOk},
                           {.name = "., SEND FAIL\r\n", .err = SIM_sendFail},
                           {.name = NULL, .err = SIM_noErrCode}};
    cmd->resp.pos_resps = &c_st;
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPSTART_TIMEOUT;

    return cmd;
}

SIM_data_len SIM_TCP_read(SIM_intf *sim, SIM_con_num n, void *buf, unsigned int len)
{
    if (n > SIM_con_5 || n < SIM_con_0)
        return SIM_err;

    SIM_error err = SIM_noErrCode;
    SIM_TCP_cmd_grip *cmd_grip;
    SIM_TCP_cmd *cmd;
    SIM_resp *resp;

    // Find the right stream
    cmd_grip = &sim->tcp_cmds[n];
    cmd = sim->tcp_cmds[n].cmd;
    if (cmd)
        resp = &sim->tcp_cmds[n].cmd->resp;
    else
        return SIM_err;
    
    // 
    err = resp->err;
    if (err != SIM_noErrCode && err != SIM_receive && err != SIM_unknown)
        return err;
    
    // no data to receive, wait for it
    err = SIM_noErrCode;
    if (resp->data_len == 0)
    {
        xQueueReceive(cmd_grip->queue, &err, SIM_TCP_READ_TIMEOUT / portTICK_PERIOD_MS);
    }

    // if error code 
    if (err != SIM_noErrCode && err != SIM_receive)
        return err;
    
    // Read the data and delete it from the buffer
    xSemaphoreTake(resp->data_mutex, portMAX_DELAY);
    unsigned int read_len;

    // static char test[2][7];

    if (len < resp->data_len)
    {
        // memcpy(test[0], resp->data, resp->data_len);
        memcpy(buf, resp->data, len);
        memcpy(resp->data, resp->data + len, resp->data_len - len);
        resp->data = realloc(resp->data, (resp->data_len = (resp->data_len - len)));
        read_len = len;
        goto EXIT;
    }
    else
    {
        if (resp->data_len < len) // TODO change so only waits when packet indycator was found in buffer  
        {   
            xSemaphoreGive(resp->data_mutex);
            // wait for enough data while raw data is still being processed by main task
            while((len > resp->data_len) && sim->rec_len);
            xSemaphoreTake(resp->data_mutex, portMAX_DELAY);
            if (!resp->data_len)
            {
                printf("NO DATA\r\n");
                read_len = 0;
                goto EXIT;
            }
            
            if (resp->data_len < len)
            {
                memcpy(buf, resp->data, resp->data_len);
                free(resp->data);
                resp->data_len = 0;
                read_len = resp->data_len;
                goto EXIT;
            }
            else
            {
                memcpy(buf, resp->data, len);
                memcpy(resp->data, resp->data + len, resp->data_len - len);
                resp->data = realloc(resp->data, (resp->data_len = (resp->data_len - len)));
                read_len = len;
                goto EXIT;
            }
        }

        // memcpy(test[1], resp->data, resp->data_len);
        else //if (len == resp->data_len)
        {
            memcpy(buf, resp->data, resp->data_len);
            if (resp->data_len != 0)
                free(resp->data);
            read_len = resp->data_len;
            resp->data_len = 0;
        }
        goto EXIT;
    }

    EXIT:

    // static int break_p = 0; 

    // for (int i = 0; i < read_len; i++)
    // {
    //     printf("%X  ", ((char *)buf)[i]);
    //     if (++break_p == 16)
    //     {
    //         printf("\r\n");
    //         break_p = 0;
    //     }
    // }

    xSemaphoreGive(resp->data_mutex);
    return read_len;
}

// SIM_data_len SIM_TCP_read(SIM_intf *sim, SIM_con_num n, void *buf, unsigned int len)
// {
//     char n_str[10];
//     sprintf(n_str, "%u", n);

//     SIM_cmd *cmd = NULL;
//     SIM_resp *resp;

//     // Find the right stream
//     for(int i = 0; i < sim->cmds_num; i++)
//     {
//         if(sim->cmds[i].cmd->at[12] == n_str[0])
//         {
//             cmd = sim->cmds[i].cmd;
//             resp = &cmd->resp;
//             break;
//         }
//     }
//     if (cmd == NULL)
//         return SIM_err;
    
//     // Read the data and delete it from the buffer
//     xSemaphoreTake(resp->data_mutex, portMAX_DELAY);
//     unsigned int read_len;
//     if (len < resp->data_len)
//     {
//         memcpy(buf, resp->data, len);
//         memcpy(resp->data, resp->data + len, resp->data_len - len);
//         resp->data_len = resp->data_len - len;
//         resp->data = realloc(resp->data, resp->data_len - len);
//         read_len = len;
//         goto EXIT;
//     }
//     else
//     {
//         memcpy(buf, resp->data, resp->data_len);
//         free(resp->data);
//         resp->data_len = 0;
//         goto EXIT;
//     }

//     EXIT:
//     xSemaphoreGive(resp->data_mutex);
//     return read_len;
// }

SIM_data_len SIM_TCP_write(SIM_intf *sim, SIM_con_num n, void *buf, unsigned int len)
{
    // printf("\r\n");
    // static unsigned char temp[1000] = {};
    // memcpy(temp, buf, len);
    // for(unsigned int i = 0; i < len; i++)
    // {
    //     printf("%u", (unsigned char)temp[i]);
    // }
    // printf("\r\n");
    
    SIM_error err;
    SIM_cmd cmd;

    if ((err = SIM_run(sim, SIM_writeCIPSEND(&cmd, n, len, buf, len))) != SIM_ok)
        return (int)err;

    return len;
}

SIM_TCP_cmd *SIM_listenTCP(SIM_TCP_cmd *cmd, const SIM_con_num n, void (*resp_handler)(SIM_error *))
{
    if (n > SIM_con_5 || n < SIM_con_def)
        return NULL;

    cmd->con = n;
    if (n == SIM_con_def)
        cmd->handler = &SIM_listenTCP_cipmux0_handler;
    else
        cmd->handler = &SIM_listenTCP_cipmux1_handler;
    if (resp_handler == NULL)
        cmd->resp_handler = &SIM_listenTCP_resp_handler;
    else 
        cmd->resp_handler = resp_handler;
    SIM_respNULL(&cmd->resp, NULL);
    cmd->resp.data_mutex = xSemaphoreCreateBinary();
    xSemaphoreGive(cmd->resp.data_mutex);

    return cmd;
}

SIM_error SIM_listenTCP_setHandler(SIM_intf *sim, const SIM_con_num n,  void (*resp_handler)(SIM_error *))
{
    if (n > SIM_con_5 || n < SIM_con_0)
        return SIM_err;

    SIM_error err = SIM_noErrCode;
    SIM_TCP_cmd_grip *cmd_grip;
    SIM_TCP_cmd *cmd;
    SIM_resp *resp;

    // Find the right stream
    cmd_grip = &sim->tcp_cmds[n];
    cmd = sim->tcp_cmds[n].cmd;
    if (!cmd)
        return SIM_err;

    cmd->resp_handler = resp_handler;

    return SIM_ok;
}

static SIM_error SIM_writeCIPMUX_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

SIM_cmd *SIM_writeCIPMUX(SIM_cmd *cmd, const unsigned char n)
{
    SIM_param params[2];
    sprintf(params[0].name, "%u", (unsigned int)n);
    *params[1].name = NULL;
    SIM_setAT(cmd->at, "CIPMUX", params);
    cmd->handlers[0] = &SIM_writeCIPMUX_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPMUX_TIMEOUT;

    return cmd;
}