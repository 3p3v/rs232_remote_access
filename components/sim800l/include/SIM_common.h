#pragma once

#include "SIM_common_def.h"
#include "LL_SIM.h"

/* Subscribe to the listener, wait for error code */
SIM_error SIM_sub(SIM_intf *sim, SIM_cmd *cmd);
SIM_error SIM_sub_multiple_launch(SIM_intf *sim, SIM_TCP_cmd *cmd);
// void SIM_unsub(SIM_intf *sim)
// {
//     sim->cmds_num--;
//     sim->cmds[sim->cmds_num].buf = NULL;
//     sim->cmds[sim->cmds_num].buf_len = NULL;
// }

SIM_error SIM_run(SIM_intf *sim, SIM_cmd *cmd);
SIM_error SIM_run_multiple_launch(SIM_intf *sim, SIM_TCP_cmd *cmd);

void SIM_setAT(char *at, const char *type, const SIM_param *params);

SIM_error SIM_sendAT(const SIM_intf *sim, const char *at); // TODO check length
// SIM_error SIM_sendAT(const SIM_intf *sim, const char *type, const char **params);
SIM_error SIM_sendAT_short(const SIM_intf *sim, const char *type);
SIM_error SIM_sendAT_null(const SIM_intf *sim);
/* Receives raw data from SIM800L, uses SIM_intf sim buffer*/

SIM_errMsgEnd_pair SIM_retrieveErr(const SIM_line_pair *lines);
/* Find custom error message, can be located anywhere */
// SIM_errMsgEnd_pair SIM_retrieveCustomErr_find(char *buf, unsigned int rec_len, const SIM_err_pair *err);
/* Get ptr and length of raw response from raw data */
SIM_error SIM_retrieveResp(SIM_line_pair *lines, const char *at, SIM_resp *resp);
// SIM_data_len SIM_retriveParam(const char *resp, SIM_data_len resp_len, const unsigned char param_num, char *out);
SIM_error SIM_retrieveParams(SIM_resp *resp);
/*  */
SIM_data_len SIM_retrieveData(char *buf, unsigned int rec_len, SIM_resp *resp, const unsigned int data_len);
/*  */
// SIM_error SIM_receive(SIM_intf *sim, SIM_time time);
/*  */
SIM_error SIM_retrieve(SIM_line_pair *lines, char *at, SIM_resp *resp);
