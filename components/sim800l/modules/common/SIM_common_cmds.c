#include <SIM_common_cmds.h>
#include <string.h>

static SIM_error SIM_execAT_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

SIM_cmd *SIM_execAT(SIM_cmd *cmd)
{
    strcpy(cmd->at, "AT\r\n");
    cmd->handlers[0] = &SIM_execAT_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECATE0_TIMEOUT;

    return cmd;
}

static SIM_error SIM_execATE0_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

SIM_cmd *SIM_execATE0(SIM_cmd *cmd)
{
    strcpy(cmd->at, "ATE0\r\n");
    cmd->handlers[0] = &SIM_execATE0_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECATE0_TIMEOUT;

    return cmd;
}

static SIM_error SIM_readCREG_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    if (resp->err != SIM_ok)
        return resp->err;

    resp->err = SIM_retrieve(lines, "CREG", resp);
    if (resp->err != SIM_ok)
        return resp->err;

    return resp->err;
}

SIM_cmd *SIM_readCREG(SIM_cmd *cmd)
{
    SIM_param params[1];
    *params[0].name = '\0';
    SIM_setAT(cmd->at, "CREG?", params);
    cmd->handlers[0] = &SIM_readCREG_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->timeout = SIM_READCREG_TIMEOUT;

    return cmd;
}

static SIM_error SIM_readCCLK_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    if (resp->err != SIM_ok)
        return resp->err;

    resp->err = SIM_retrieve(lines, "CCLK", resp);
    if (resp->err != SIM_ok)
        return resp->err;

    resp->params[0].ptr += 2;
    resp->params[0].len = 17;
    resp->params_num = 1;

    return resp->err;
}

SIM_cmd *SIM_readCCLK(SIM_cmd *cmd)
{
    SIM_param params[1];
    *params[0].name = '\0';
    SIM_setAT(cmd->at, "CCLK?", params);
    cmd->handlers[0] = &SIM_readCCLK_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_READCCLK_TIMEOUT;

    return cmd;
}

static SIM_error SIM_writeCLTS_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    if (resp->err != SIM_ok)
        return resp->err;

    resp->err = SIM_retrieve(lines, "CLTS", resp);
    if (resp->err != SIM_ok)
        return resp->err;

    return resp->err;
}

SIM_cmd *SIM_writeCLTS(SIM_cmd *cmd, unsigned char mode)
{
    SIM_param params[1];
    sprintf(params[0].name, "%u", (unsigned int)mode);
    SIM_setAT(cmd->at, "CLTS", params);
    cmd->handlers[0] = &SIM_writeCLTS_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_READCCLK_TIMEOUT;

    return cmd;
}

static SIM_error SIM_execATandW_handler(SIM_line_pair *lines, SIM_line_pair *lines_beg, SIM_resp *resp, void *sim)
{
    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    if (resp->err != SIM_ok)
        return resp->err;

    return resp->err;
}

SIM_cmd *SIM_execATandW(SIM_cmd *cmd)
{
    SIM_param params[1];
    *params[0].name = '\0';
    SIM_setAT(cmd->at, "AT&W", params);
    cmd->handlers[0] = &SIM_readCCLK_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_READCCLK_TIMEOUT;

    return cmd;
}