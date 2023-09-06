#pragma once

#include "SIM_common_def.h"
#include "LL_SIM.h"

/* Subscribe to the listener, wait for error code */
SIM_error SIM_sub(SIM_intf *sim, SIM_cmd *cmd);

// void SIM_unsub(SIM_intf *sim)
// {
//     sim->cmds_num--;
//     sim->cmds[sim->cmds_num].buf = NULL;
//     sim->cmds[sim->cmds_num].buf_len = NULL;
// }

SIM_error SIM_at(SIM_intf *sim, SIM_cmd *cmd);



SIM_error SIM_sendAT(const SIM_intf *sim, const char *type, const char **params);
// SIM_error SIM_sendAT(const SIM_intf *sim, const char *type, const char **params);
SIM_error SIM_sendAT_short(const SIM_intf *sim, const char *type);
SIM_error SIM_sendAT_null(const SIM_intf *sim);
/* Receives raw data from SIM800L, uses SIM_intf sim buffer*/

SIM_error SIM_retrieveErr(char *buf, unsigned int rec_len);
SIM_error SIM_retrieveErr_find(const SIM_intf *sim);
/* Find custom error message, can be located anywhere */
SIM_error SIM_retrieveCustomErr_find(const SIM_intf *sim, const SIM_err_pair *err);
/* Get ptr and length of raw response from raw data */
SIM_error SIM_retrieveResp(char *buf, unsigned int rec_len, SIM_resp *resp);
SIM_error SIM_retrieveCustomResp(const SIM_intf *sim, SIM_resp *resp, const char *resp_name);
// SIM_data_len SIM_retriveParam(const char *resp, SIM_data_len resp_len, const unsigned char param_num, char *out);
SIM_error SIM_retrieveParams(SIM_resp *resp);
/*  */
SIM_data_len SIM_retrieveData(char *buf, unsigned int rec_len, SIM_resp *resp, const unsigned int data_len);
/*  */
// SIM_error SIM_receive(SIM_intf *sim, SIM_time time);
/*  */
SIM_error SIM_retrieve(char *buf, unsigned int rec_len, SIM_resp *resp);
