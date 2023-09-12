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
            resp->data_len = end - resp->data;
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
    // todo check code and connect status
    return err.err;
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

SIM_cmd *SIM_execCIPSEND(SIM_cmd *cmd, char *send_data, SIM_data_len send_data_len)
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

    SIM_err_pair c_st[] = {{.name = "., SEND OK\r\n", .err = SIM_sendOk},
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

SIM_cmd *SIM_writeCIPSEND(SIM_cmd *cmd, SIM_con_num n, SIM_data_len length, char *send_data, SIM_data_len send_data_len)
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
    cmd->resp.send_data = send_data;
    cmd->resp.send_data_len = send_data_len;
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPSEND_TIMEOUT;

    return cmd;
}

static void SIM_listenTCP_receive_handler(/* EDIT */)
{
    /* EDIT */

    /********/
}

static void SIM_listenTCP_closed_handler(/* EDIT */)
{
    /* EDIT */

    /********/
}

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
        SIM_listenTCP_closed_handler(/* EDIT */);
        resp->err = err.err;
        resp->msg_end = err.ptr;
        return SIM_err;
    }
    else
    {
        SIM_listenTCP_receive_handler(/* EDIT */);
        resp->data = buf;
        resp->err = err.err;
        err = SIM_retrieveCustomErr(lines, SIM_reservedResps);
        resp->data_len = err.ptr - resp->data;
        resp->msg_end = err.ptr;

        return SIM_ok;
    }
}

static SIM_error SIM_listenTCP_cipmux1_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    resp->data = NULL;
    resp->data_len = 0;
    resp->params_num = 0;
    resp->resp_name_len = 0;

    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_data_len limes_num = SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

    SIM_err_pair c_st[] = {{.name = "+RECEIVE,", .err = SIM_receive},
                           {.name = "CLOSED\r\n", .err = SIM_closed},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr(lines, c_st);

    // check if data was received from the TCP connection
    if (err.err == SIM_receive)
    {
        // check if it's the right stream
        if (resp->at[12] != err.ptr[0])
        {
            return SIM_noErrCode;
        }
        
        // get parameters
        resp->params[0].ptr = err.ptr;
        resp->params[0].len = 1;
        resp->params[1].ptr = resp->params[0].ptr + 2;
        resp->params[1].len = strstr(resp->params[1].ptr, ":") - resp->params[1].ptr;
        resp->params_num = 2;

        // get data
        SIM_line_pair *lines_ptr = NULL;
        for (unsigned int i = 0; i < limes_num; i++)
        {
            if (lines[i].ptr == err.ptr_beg)
            {
                lines_ptr = (lines + i);
            }
        }
        SIM_errMsgEnd_pair err2 = SIM_retrieveCustomErr(lines_ptr, SIM_reservedResps);
        resp->msg_end = err2.ptr;

        SIM_listenTCP_receive_handler(/* EDIT */);
        resp->err = SIM_receive;
        return SIM_ok;
    }
    else if (err.err == SIM_closed)
    {
        SIM_listenTCP_closed_handler(/* EDIT */);
        resp->err = SIM_closed;
        resp->err = err.err;
        return SIM_err;
    }
    else
    {
        // response from SIM doesn't match any code
        return SIM_noErrCode;
    }
}

SIM_cmd * SIM_listenTCP(SIM_cmd *cmd, const SIM_con_num n)
{
    if (n > SIM_con_5 || n < SIM_con_def)
        return NULL;

    strcpy(cmd->at, "LISTEN_TCP: ");

    const char n_str[10] = {};
    sprintf(n_str, "%u", (unsigned int)n);
    strcat(cmd->at, n_str);

    if (n == SIM_con_def)
        cmd->handlers[0] = &SIM_listenTCP_cipmux0_handler;
    else
        cmd->handlers[0] = &SIM_listenTCP_cipmux1_handler;

    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_multiple_launch;
    cmd->timeout = 0;

    return cmd;
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