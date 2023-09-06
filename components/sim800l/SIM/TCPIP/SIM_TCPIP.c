#include "SIM_TCPIP.h"
#include <string.h>

void SIM_readCGATT(SIM_cmd *cmd)
{
    strcpy(cmd->at, "CGATT?");
    SIM_paramsNULL(cmd->params);
    cmd->params_num = 0;
    cmd->handlers[0] = &SIM_readCGATT_handler;
    cmd->handlers_num = 0;
    SIM_respNULL(&cmd->resp);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_READCGATT_TIMEOUT;
    cmd->err = SIM_unknown;
}

SIM_error SIM_readCGATT_handler(char *buf, unsigned int rec_len, SIM_resp *resp)
{
    SIM_error err = SIM_retrieveErr(buf, rec_len);

    err = SIM_retrieve(buf, rec_len, resp);
    if (err != SIM_ok)
        return err;

    return err;
}