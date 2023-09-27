#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

/* Must exist, can be changed */
#define SIM_MAX_PARAMS 4
#define SIM_MAX_AT_LEN 50
#define SIM_MAX_PARAM_LEN 30
#define SIM_MAX_HANDLERS_NUM 10
#define SIM_MAX_AT_STR_LEN (SIM_MAX_AT_LEN + 1)
#define SIM_MAX_LINES 20
#define SIM_MAX_RESPS 10
#define SIM_MAX_LINES_ARR_LEN (SIM_MAX_LINES + 1)
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
    SIM_noErrCode = -8,
    SIM_connectOk = -9,
    SIM_alreadyConnect = -10,
    SIM_connectFail = -11,
    SIM_closed = -12,
    SIM_closedOk = -13,
    SIM_sendOk = -14,
    SIM_receive = -15,
    SIM_sendFail = -16,
    SIM_smsReady = -17,
    SIM_callReady = -18,
    SIM_ring = -19,
    sim_msgDetect = -20,
    SIM_unknown = -99
    // SIM_registered = -7,
    // SIM_roamingRegistered = -8,
    // SIM_serverErr = -9
} SIM_error;
typedef SIM_error LL_SIM_error;



typedef struct SIM_respe_params
{
    char *ptr;
    unsigned char len; 
} SIM_resp_params;

typedef struct SIM_err_pair
{
    const char *name;
    const SIM_error err; 
} SIM_err_pair;

// typedef struct SIM_err_triple
// {
//     const char *name;
//     const SIM_error err; 
//     const unsigned char max_len;
// } SIM_err_triple;

typedef struct SIM_line_pair
{
    char *ptr;
    unsigned int len; 
} SIM_line_pair;
// typedef union SIM_err_pair_triple
// {
//     SIM_err_triple 
// } SIM_err_pair_triple;

typedef struct SIM_errMsgEnd_pair
{
    char *ptr_beg;
    char *ptr;
    SIM_error err; 
    unsigned int line_num;
} SIM_errMsgEnd_pair;

typedef struct SIM_resp_pair
{
    char *ptr_beg;
    char *ptr;
    unsigned int line_num;
} SIM_resp_pair;

typedef struct SIM_param
{
    char name[SIM_MAX_PARAM_LEN];
} SIM_param;

typedef struct SIM_resp
{
    SIM_err_pair *pos_resps;
    char at[SIM_MAX_AT_STR_LEN];
    char *resp_name;    //TODO delete??
    char *resp_name_len;
    char *resp;
    unsigned char resp_len;
    unsigned char params_num;
    SIM_resp_params params[SIM_MAX_PARAMS];
    void *send_data;
    unsigned int send_data_len;
    // unsigned char *data_buf;
    SemaphoreHandle_t data_mutex;
    void *data;
    unsigned int data_len;
    char *msg_end;
    SIM_error err;
} SIM_resp;

typedef enum SIM_cmd_type
{
    SIM_cmd_single_use = 0,
    SIM_cmd_multiple_launch
} SIM_cmd_type;

typedef unsigned int SIM_time;
// typedef struct 
// {
//     /* Buffer used for receiving raw messages from SIM800L module */
//     char *buf;// [LL_SIM_DEF_BUF_SIZE];
//     unsigned int buf_len;
//     /* Length of last received message from SIM800L module */
//     unsigned int rec_len;
//     // SIM_response resp;
// } SIM_cont;

// typedef struct SIM_handler
// {
//     SIM_error (*func)(char *, unsigned int, SIM_resp *);
//     int timeout;
// } SIM_handler;

typedef enum SIM_con_num 
{
    SIM_con_def = - 1,
    SIM_con_0,
    SIM_con_1,
    SIM_con_2,
    SIM_con_3,
    SIM_con_4,
    SIM_con_5
} SIM_con_num;

typedef struct SIM_cmd
{
    char at[SIM_MAX_AT_STR_LEN];
    // char params[SIM_MAX_PARAMS][SIM_MAX_PARAM_LEN];
    // unsigned char params_num;
    SIM_error (*handlers[SIM_MAX_HANDLERS_NUM])(char *, unsigned int, SIM_resp *, void *);
    unsigned char handlers_num;
    SIM_resp resp;
    SIM_cmd_type type; // TODO delete
    SIM_time timeout;
    // SIM_error err;
} SIM_cmd;

typedef struct SIM_TCP_cmd
{
    SIM_con_num con;
    SIM_error (*handler)(char *, unsigned int, SIM_resp *, void *);
    SIM_resp resp;
    void (*resp_handler)(SIM_error *err);
} SIM_TCP_cmd;



/* Reset response to NULL */
void SIM_respNULL(SIM_resp* resp, const char *at_resp_name);
void SIM_paramsNULL(char params[SIM_MAX_PARAMS][SIM_MAX_PARAM_LEN]);

/* cstring parameters to numbers */
unsigned char SIM_atoi_uint8_t(const char *param, unsigned char param_len);
unsigned int SIM_atoi_uint32_t(const char *param, unsigned char param_len);
int SIM_atoi_int32_t(const char *param, unsigned char param_len);

typedef int SIM_data_len;

SIM_data_len SIM_findAllLines(const void *buf, const SIM_data_len rec_len, SIM_line_pair *lines, unsigned int lines_num);
SIM_errMsgEnd_pair SIM_retrieveCustomErr(const SIM_line_pair *lines, const SIM_err_pair *errs);
SIM_resp_pair SIM_retrieveCustomResp(const SIM_line_pair *lines, const char *resp);

typedef enum SIM_pin
{
    SIM_pinLow = 0,
    SIM_pinHigh = 1
} SIM_pin;
typedef SIM_pin LL_SIM_pin;


