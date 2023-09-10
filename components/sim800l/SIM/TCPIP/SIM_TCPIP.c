#include "SIM_TCPIP.h"
#include <string.h>

static SIM_error SIM_readCGATT_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(buf, rec_len);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    if (resp->err != SIM_ok)
        return resp->err;

    resp->err = SIM_retrieve(buf, rec_len, resp);
    if (resp->err != SIM_ok)
        return resp->err;

    return resp->err;
}

void SIM_readCGATT(SIM_cmd *cmd)
{
    strcpy(cmd->at, "CGATT?");
    SIM_paramsNULL(cmd->params);
    cmd->params_num = 0;
    cmd->handlers[0] = &SIM_readCGATT_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_READCGATT_TIMEOUT;
}

static SIM_error SIM_writeCSTT_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(buf, rec_len);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

void SIM_writeCSTT(SIM_cmd *cmd, const char *apn, const char *username, const char *password)
{
    strcpy(cmd->at, "CSTT");
    SIM_paramsNULL(cmd->params);
    strcpy(cmd->params[0], apn);
    if (username && password)
    {
        strcpy(cmd->params[1], username);
        strcpy(cmd->params[2], password);
        cmd->params_num = 3;
    }
    else
    {
        cmd->params_num = 1;
    }
    cmd->handlers[0] = &SIM_writeCSTT_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECSTT_TIMEOUT;
}

static SIM_error SIM_execCIICR_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(buf, rec_len);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

void SIM_execCIICR(SIM_cmd *cmd)
{
    strcpy(cmd->at, "CIICR");
    SIM_paramsNULL(cmd->params);
    cmd->params_num = 0;
    cmd->handlers[0] = &SIM_execCIICR_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECCIICR_TIMEOUT;
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
    SIM_errMsgEnd_pair err = SIM_retrieveErr(buf, rec_len);
    resp->err = err.err;
    if (resp->err != SIM_noErrCode)
        return resp->err;

    err = SIM_execCIFSR_retrieveIP(buf, rec_len, resp);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    
    return err.err;
}

void SIM_execCIFSR(SIM_cmd *cmd)
{
    strcpy(cmd->at, "CIFSR");
    SIM_paramsNULL(cmd->params);
    cmd->params_num = 0;
    cmd->handlers[0] = &SIM_execCIFSR_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECCIICR_TIMEOUT;
}

static SIM_error SIM_writeCIPSTART_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr_find(buf, rec_len);
    if (err.err != SIM_ok && err.err != SIM_err)
        return SIM_noErrCode;

    SIM_err_pair c_st[] = {{.name = "\r\nCONNECT OK\r\n", .err = SIM_connectOk},
                           {.name = "\r\nALREADY CONNECT\r\n", .err = SIM_alreadyConnect},
                           {.name = "\r\nCONNECT FAIL\r\n", .err = SIM_connectFail},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err2 = SIM_retrieveCustomErr_find(buf, rec_len, c_st);
    resp->err = err2.err;
    resp->msg_end = err2.ptr;
    if (resp->err != SIM_connectOk && resp->err != SIM_alreadyConnect && resp->err != SIM_connectFail)
        return SIM_noErrCode;
    // todo check code and connect status
    return err.err;
}

void SIM_writeCIPSTART(SIM_cmd *cmd, const SIM_con_num n, char *mode, char *address, const unsigned int port)
{
    strcpy(cmd->at, "CIPSTART");
    SIM_paramsNULL(cmd->params);

    unsigned char num = 0;
    if (n != SIM_con_def)
    {
        sprintf(cmd->params[0], "%i", (int)n);
        num++;
    }
    strcpy(cmd->params[0 + num], mode);
    strcpy(cmd->params[1 + num], address);
    sprintf(cmd->params[2 + num], "%u", (unsigned int)port);
    cmd->params_num = 3 + num;

    cmd->handlers[0] = &SIM_writeCIPSTART_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPSTART_TIMEOUT;
}

static SIM_error SIM_execCIPSEND_handler2(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{

    SIM_err_pair c_st[] = {{.name = "\r\nSEND OK\r\n", .err = SIM_sendOk},
                           {.name = "\r\nSEND FAIL\r\n", .err = SIM_sendFail},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr_find(buf, rec_len, c_st);
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

    // SIM_err_pair c_st[] = {{.name = ">", .err = SIM_ok},
    //                        {.name = NULL, .err = SIM_ok}};

    // SIM_error err = SIM_retrieveCustomErr_find(buf, rec_len, c_st);
    SIM_err_pair c_st[] = {{.name = "> ", .err = SIM_ok},
                           {.name = "\r\nERROR\r\n", .err = SIM_err},
                           {.name = NULL, .err = SIM_noErrCode}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr_find(buf, rec_len, c_st);
    resp->err = err.err;
    resp->msg_end = err.ptr;

    if (err.err == SIM_ok)
        err.err = LL_SIM_sendData((SIM_intf *)sim, resp->send_data);

    if (err.err == SIM_ok)
        err.err = LL_SIM_sendData((SIM_intf *)sim, "\032");

    return err.err;
}

void SIM_execCIPSEND(SIM_cmd *cmd, char *send_data, char *send_data_len)
{
    strcpy(cmd->at, "CIPSEND");
    SIM_paramsNULL(cmd->params);
    cmd->params_num = 0;
    cmd->handlers[0] = &SIM_execCIPSEND_handler2;
    cmd->handlers[1] = &SIM_execCIPSEND_handler1;
    cmd->handlers_num = 2;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->resp.send_data = send_data;
    cmd->resp.send_data_len = send_data_len;
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_WRITECIPSEND_TIMEOUT;
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
    char *beg;

    beg = buf;


    SIM_err_pair c_st[] = {{.name = "ERROR\r\n", .err = SIM_closed},
                           {.name = NULL, .err = SIM_receive}};

    SIM_errMsgEnd_pair err = SIM_retrieveCustomErr_find(buf, rec_len, c_st);
    
    if (err.err == SIM_closed)
    {
        SIM_listenTCP_closed_handler(/* EDIT */);
        resp->err = err.err;
        resp->msg_end = err.ptr;
        return SIM_err;
    }
    else
    {
        resp->data = buf;
        resp->data_len = strlen(resp->data) - strlen("\r\n");
        resp->err = err.err;
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
    char *beg;

    char str[10] = "+RECEIVE,";
    beg = strstr(buf, str);

    // check if data was received from the TCP connection
    if (beg)
    {
        // get parameters
        resp->params[0].ptr = beg + sizeof(str);
        resp->params[0].len = 1;
        resp->params[1].ptr = beg + sizeof(str) + 2;
        resp->params[1].len = strstr(resp->params[1].ptr, ":") - resp->params[1].ptr;
        resp->params_num = 2;

        // get data
        resp->data = resp->params[1].ptr + resp->params[1].len + sizeof(":\r\n");
        resp->data_len = strstr(resp->data, "\0") - resp->data;

        SIM_listenTCP_receive_handler(/* EDIT */);
        resp->err = SIM_receive;
        return SIM_ok;
    }

    memset(str, '\0', sizeof(char) * 10);
    strcpy(str, "\r\n");
    char n_str[6];
    strcat(str, resp->at + 12);
    strcat(str, ", ");
    beg = strstr(buf, str);

    // if NULL then no data was found for this stream
    if (beg == NULL)
        return SIM_unknown;

    // check if data was succesfully sent from the module
    // if (strstr(beg, "SEND OK"))
    // {
    //     resp->data = beg + strlen("SEND OK") + strlen("\r\n");
    //     SIM_listenTCP_sendOk_handler(/* EDIT */);
    //     return SIM_sendOk;
    // }
    if (strstr(beg, "CLOSED\r\n"))
    {
        SIM_listenTCP_closed_handler(/* EDIT */);
        resp->err = SIM_closed;
        return SIM_err;
    }
    else
    {
        // response from SIM doesn't match any code
        return SIM_unknown;
    }
}

void SIM_listenTCP(SIM_cmd *cmd, const SIM_con_num n)
{
    strcpy(cmd->at, "LISTEN_TCP: ");

    const char n_str[10] = {};
    sprintf(n_str, "%u", n);
    strcat(cmd->at, n_str);

    SIM_paramsNULL(cmd->params);
    cmd->params_num = 0;
    if (n == 0)
        cmd->handlers[0] = &SIM_listenTCP_cipmux0_handler;
    else
        cmd->handlers[0] = &SIM_listenTCP_cipmux1_handler;

    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_multiple_launch;
    cmd->timeout = 0;
}
