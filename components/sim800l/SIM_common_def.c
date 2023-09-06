#include "SIM_common_def.h"
#include <stddef.h>
#include <string.h>

void SIM_respNULL(SIM_resp *resp)
{
    resp->resp = NULL;
    resp->resp_len = 0;
    resp->params_num = 0;
    memset(resp->params, '\0', SIM_MAX_PARAMS * sizeof(char*));
    resp->data = NULL;
    resp->data_len = 0;
}

void SIM_paramsNULL(char params[SIM_MAX_PARAMS][SIM_MAX_PARAM_LEN])
{
    for(int i = 0; i < SIM_MAX_PARAMS; i++)
    {
        params[i][0] = '\0';
    }
}

unsigned char SIM_atoi_uint8_t(const char *param, unsigned char param_len)
{
    char param_str[6] = {};
    memcpy(param_str, param, param_len);
    return (unsigned char)atoi(param_str);
}

unsigned int SIM_atoi_uint32_t(const char *param, unsigned char param_len)
{
    char param_str[6] = {};
    memcpy(param_str, param, param_len);
    return (unsigned int)atoi(param_str);
}

int SIM_atoi_int32_t(const char *param, unsigned char param_len)
{
    char param_str[7] = {};
    memcpy(param_str, param, param_len);
    return atoi(param_str);
}

