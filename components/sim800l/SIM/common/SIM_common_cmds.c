#include <SIM_common_cmds.h>
#include <string.h>

static SIM_error SIM_execCIICR_handler(char *buf, unsigned int rec_len, SIM_resp *resp, void *sim)
{
    SIM_line_pair lines[SIM_MAX_LINES_ARR_LEN];
    SIM_findAllLines(buf, rec_len, lines, SIM_MAX_LINES_ARR_LEN);

    SIM_errMsgEnd_pair err = SIM_retrieveErr(lines);
    resp->err = err.err;
    resp->msg_end = err.ptr;
    return resp->err;
}

SIM_cmd *SIM_execATE0(SIM_cmd *cmd)
{
    strcpy(cmd->at, "ATE0\r\n");
    cmd->handlers[0] = &SIM_execCIICR_handler;
    cmd->handlers_num = 1;
    SIM_respNULL(&cmd->resp, cmd->at);
    cmd->type = SIM_cmd_single_use;
    cmd->timeout = SIM_EXECATE0_TIMEOUT;

    return cmd;
}