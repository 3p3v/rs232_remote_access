#pragma once

/* Must exist, can be changed */
#define SIM_MAX_PARAMS 4
#define SIM_MAX_AT_LEN 10
#define SIM_MAX_AT_STR_LEN SIM_MAX_AT_LEN + 1
#define SIM_CHECK_OK

/* Error codes */
typedef enum SIM_error
{
    SIM_ok = 0,                     /* Ok code received */
    SIM_err = -1,                   /* Error code received */
    SIM_recErr = -2,                /* Error receiving message */
    SIM_bufferFullErr = -3,         /* Error receiving message, buffer full detected */
    SIM_hardwareErr = -4,           /* Error receiving message, hardware interface error detected */
    SIM_timeoutErr = -5,            /* Error receiving message, timeout detected */
    SIM_respErr = -6,
    SIM_noResp = -7,
    SIM_noErrCode = -8
    // SIM_registered = -7,
    // SIM_roamingRegistered = -8,
    // SIM_serverErr = -9
} SIM_error;
typedef SIM_error LL_SIM_error;

typedef struct SIM_response_params
{
    char *ptr;
    unsigned char len; 
} SIM_response_params;

typedef struct SIM_response_params
{
    char *name_ptr;
    SIM_error err; 
} SIM_err_map;

typedef struct SIM_resp
{
    char *resp;
    unsigned char resp_len;
    unsigned char params_num;
    SIM_response_params params[SIM_MAX_PARAMS];
    char *data;
    unsigned int data_len;
} SIM_resp;

/* Reset response to NULL */
void SIM_respNull(SIM_resp* resp);

/* cstring parameters to numbers */
unsigned char SIM_atoi_uint8_t(const char *param, unsigned char param_len);
unsigned int SIM_atoi_uint32_t(const char *param, unsigned char param_len);
int SIM_atoi_int32_t(const char *param, unsigned char param_len);

typedef enum SIM_pin
{
    SIM_pinLow = 0,
    SIM_pinHigh = 1
} SIM_pin;
typedef SIM_pin LL_SIM_pin;

typedef int SIM_data_len;

typedef int SIM_time;

