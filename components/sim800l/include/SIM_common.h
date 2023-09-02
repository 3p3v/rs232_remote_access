#pragma once

#include "SIM_common_def.h"
#include "LL_SIM.h"

typedef LL_SIM_int SIM_int;

SIM_error SIM_sendAT(const SIM_int *sim, const char *type, const char **params);
// SIM_error SIM_sendAT(const SIM_int *sim, const char *type, const char **params);
SIM_error SIM_sendAT_short(const SIM_int *sim, const char *type);
SIM_error SIM_sendAT_null(const SIM_int *sim);
/* Receives raw data from SIM800L, uses SIM_int sim buffer*/
SIM_error SIM_receiveRaw(SIM_int *sim, SIM_time time);
SIM_error SIM_retrieveErr(const SIM_int *sim);
SIM_error SIM_retrieveErr_find(const SIM_int *sim);
/* Find custom error message, can be located anywhere */
SIM_error SIM_retrieveCustomErr_find(const SIM_int *sim, const SIM_err_map *err);
/* Get ptr and length of raw response from raw data */
SIM_error SIM_retrieveResp(const SIM_int *sim, SIM_resp *resp);
// SIM_data_len SIM_retriveParam(const char *resp, SIM_data_len resp_len, const unsigned char param_num, char *out);
SIM_error SIM_retrieveParams(SIM_resp *resp);
/*  */
SIM_error SIM_retrieveData(const SIM_int *sim, SIM_resp *resp, const unsigned int data_len);
/*  */
SIM_error SIM_receive(SIM_int *sim, SIM_time time);
/*  */
SIM_error SIM_retrieve(const SIM_int *sim, SIM_resp *resp);

