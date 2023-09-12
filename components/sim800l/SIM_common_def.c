#include "SIM_common_def.h"
#include <stddef.h>
#include <string.h>

const SIM_err_pair SIM_reservedResps[] = {{.name = "+RECEIVE", .err = SIM_receive},
                                          {.name = "SMS READY", .err = SIM_smsReady},
                                          {.name = "Call READY", .err = SIM_callReady},
                                          {.name = "RING", .err = SIM_ring},
                                          {.name = NULL, .err = SIM_noErrCode}};

void SIM_respNULL(SIM_resp *resp, const char *at_resp_name)
{
    memset(resp, 0, sizeof(*resp));
    strcpy(resp->at, at_resp_name);
    if (resp->at[strlen(resp->at) - 1] == '?')
        resp->at[strlen(resp->at) - 1] = '\0';

    resp->err = SIM_unknown;
    // resp->resp = NULL;
    // resp->resp_len = 0;
    // resp->params_num = 0;
    // memset(resp->params, '\0', SIM_MAX_PARAMS * sizeof(char*));
    // resp->data = NULL;
    // resp->data_len = 0;
}

void SIM_paramsNULL(char params[SIM_MAX_PARAMS][SIM_MAX_PARAM_LEN])
{
    for (int i = 0; i < SIM_MAX_PARAMS; i++)
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

SIM_data_len SIM_findAllLines(const char *buf, const SIM_data_len rec_len, SIM_line_pair *lines, unsigned int lines_num)
{
    if (lines_num < 2)
        return SIM_err;

    unsigned int i = 0;
    char *endl = strstr(buf, "\r\n");
    lines[0].ptr = buf;

    if (endl != NULL)
    {
        lines[0].len = endl++ - lines[0].ptr + strlen("\r\n");
        i++;
    }
    else
    {
        lines[0].len = 0;
        return i;
    }

    for (;;)
    {
        if ((i) == lines_num)
        {
            lines[i].ptr = NULL;
            lines[i].len = 0;
            return i;
        }

        endl = strstr(endl, "\r\n");

        if (!endl)
        {
            if ((lines[i - 1].ptr + lines[i - 1].len) != (buf + rec_len))
            {
                lines[i].ptr = lines[i - 1].ptr + lines[i - 1].len;
                lines[i].len = rec_len - (lines[i].ptr - buf);
                i++;
            }

            lines[i].ptr = NULL;
            lines[i].len = 0;
            return i;
        }
        else
        {
            lines[i].ptr = lines[i - 1].ptr + lines[i - 1].len;
            lines[i].len = endl++ - lines[i].ptr + strlen("\r\n");
            i++;
        }
    }
}

/* Checks if string line == string err */
static char *SIM_util_lineerr(const SIM_line_pair *line, const char *err)
{
    if (strlen(err) > line->len)
        return NULL;

    char *ptr = NULL;

    for (unsigned int i = 0; i < strlen(err); i++)
    {
        if (err[i] == '.')
        {
            i++;
        }
        else if (err[i] != line->ptr[i])
        {
            return NULL;
        }
    }

    ptr = line->ptr;
    return ptr;
}

/* Finds the first ocurence of any of err strings in line strings */
SIM_errMsgEnd_pair SIM_retrieveCustomErr(const SIM_line_pair *lines, const SIM_err_pair *errs)
{
    SIM_line_pair *line_ptr = &lines[0]; // = &lines[0];
    SIM_err_pair *err_ptr = &errs[0];
    SIM_errMsgEnd_pair pair; // = {.ptr = ptr + line_ptr->len, .err = err_ptr->err};
    char *ptr;
    unsigned int line_num = 0;

    if (line_ptr->ptr == NULL)
    {
        pair.ptr_beg = NULL;
        pair.ptr = NULL;
        pair.line_num = 0;
        return pair;
    }

    do
    {
        
        
        do
        {
            ptr = SIM_util_lineerr(line_ptr, err_ptr->name);

            if (ptr != NULL)
            {
                pair.ptr_beg = line_ptr->ptr;
                pair.ptr = ptr + line_ptr->len;
                pair.err = err_ptr->err;
                pair.line_num = line_num;
                return pair;
            }

            line_ptr++;
            line_num++;
        } while (line_ptr->ptr);

        line_ptr = &lines[0];
        line_num = 0;
        err_ptr++;
    } while (err_ptr->name);

    pair.ptr_beg = NULL;
    pair.ptr = NULL;
    pair.err = err_ptr->err;
    pair.line_num = 0;
    return pair;
}

/* Finds the first ocurence of any of err strings in line strings */
SIM_resp_pair SIM_retrieveCustomResp(const SIM_line_pair *lines, const char *resp)
{
    SIM_line_pair *line_ptr = &lines[0]; // = &lines[0];
    SIM_resp_pair pair;                  // = {.ptr = ptr + line_ptr->len, .err = resp_ptr->err};
    char *ptr;
    unsigned int line_num = 0;

    if (line_ptr->ptr == NULL)
    {
        pair.ptr_beg = NULL;
        pair.ptr = NULL;
        pair.line_num = 0;
        return pair;
    }

    do
    {
        ptr = SIM_util_lineerr(line_ptr, resp);

        if (ptr != NULL)
        {
            pair.ptr_beg = line_ptr->ptr;
            pair.ptr = ptr + line_ptr->len;
            pair.line_num = line_num;
            return pair;
        }

        line_ptr++;
        line_num++;
    } while (line_ptr->ptr);

    // line_num = 0;

    pair.ptr_beg = NULL;
    pair.ptr = NULL;
    pair.line_num = 0;
    return pair;
}
