#pragma once

#include "SIM_common.h"

/* AT+CGATT Attach or Detach from GPRS Service */
#define SIM_READCGATT_TIMEOUT 100

SIM_cmd *SIM_readCGATT(SIM_cmd *cmd);
// SIM_error SIM_readCGATT_handler(char *buf, unsigned int rec_len, SIM_resp *resp, SIM_intf* sim);

/* AT+CSTT Start Task and Set APN, USER NAME, PASSWORD */
#define SIM_WRITECSTT_TIMEOUT 1000
/* AT+CSTT Start Task and Set APN, USER NAME, PASSWORD */
SIM_cmd *SIM_writeCSTT(SIM_cmd *cmd, const char *apn, const char *username, const char *password);
// SIM_error SIM_writeCSTT_handler(char *buf, unsigned int rec_len, SIM_resp *resp, SIM_intf* sim);

/* AT+CIICR Bring Up Wireless Connection with GPRS or CSD */
#define SIM_EXECCIICR_TIMEOUT 10000
/* AT+CIICR Bring Up Wireless Connection with GPRS or CSD */
SIM_cmd *SIM_execCIICR(SIM_cmd *cmd);
// SIM_error SIM_execCIICR_handler(char *buf, unsigned int rec_len, SIM_resp *resp, SIM_intf* sim);

/* AT+CIFSR Get Local IP Address */
#define SIM_EXECCIFSR_TIMEOUT 85000
/* AT+CIFSR Get Local IP Address */
SIM_cmd *SIM_execCIFSR(SIM_cmd *cmd);
// SIM_error SIM_execCIFSR_handler(char *buf, unsigned int rec_len, SIM_resp *resp, SIM_intf* sim);

/* AT+CIPSTART Start Up TCP or UDP Connection */
#define SIM_WRITECIPSTART_TIMEOUT 75000
/* AT+CIPSTART Start Up TCP or UDP Connection */
SIM_cmd *SIM_writeCIPSTART(SIM_cmd *cmd, const SIM_con_num n, char *mode, char *address, const unsigned int port);
// SIM_error SIM_writeCIPSTART_handler(char *buf, unsigned int rec_len, SIM_resp *resp, SIM_intf* sim);

/* AT+CIPCLOSE Close TCP or UDP Connection */
#define SIM_WRITECIPSTART_TIMEOUT 5000
/* AT+CIPCLOSE Close TCP or UDP Connection */
SIM_cmd *SIM_writeCIPCLOSE(SIM_cmd *cmd, const SIM_con_num id, const char n);

/* AT+CIPSEND Send Data Through TCP or UDP Connection */
#define SIM_EXECCIPSEND_TIMEOUT 5000
#define SIM_WRITECIPSEND_TIMEOUT 10000
/* AT+CIPSEND Send Data Through TCP or UDP Connection */
SIM_cmd *SIM_execCIPSEND(SIM_cmd *cmd, void *send_data, SIM_data_len send_data_len);
SIM_cmd *SIM_writeCIPSEND(SIM_cmd *cmd, SIM_con_num n, SIM_data_len length, void *send_data, SIM_data_len send_data_len);
// SIM_error SIM_execCIPSEND_handler(char *buf, unsigned int rec_len, SIM_resp *resp, SIM_intf* sim);

/* Listen to TCP stream */
void SIM_listenTCP_resp_handler(SIM_error *err);
#define SIM_TCP_READ_TIMEOUT 5000
SIM_data_len SIM_TCP_read(SIM_intf *sim, SIM_con_num n, void *buf, unsigned int len);
SIM_data_len SIM_TCP_write(SIM_intf *sim, SIM_con_num n, void *buf, unsigned int len);
SIM_TCP_cmd *SIM_listenTCP(SIM_TCP_cmd *cmd, const SIM_con_num n, void (*resp_handler)(SIM_error *));
SIM_error SIM_listenTCP_setHandler(SIM_intf *sim, const SIM_con_num n,  void (*resp_handler)(SIM_error *));
/* AT+CIPMUX Start Up Multi-IP Connection */
#define SIM_WRITECIPMUX_TIMEOUT 100
SIM_cmd *SIM_writeCIPMUX(SIM_cmd *cmd, const unsigned char n);

// /* AT+CIPSTART Start Up TCP or UDP Connection */
// SIM_error SIM_writeCIPSTART(SIM_int *sim, SIM_resp *resp, const SIM_con_num n, char *mode, char *address, const unsigned int port)
// {
//     char n_str[6] = {};
//     char port_str[6] = {};
//     sprintf(port_str, "%u", (unsigned int)port);
//     char *params[5] = {NULL, NULL, NULL, NULL, NULL};
//     unsigned char num = 0;

//     if (n != SIM_con_def)
//     {
//         sprintf(n_str, "%i", (int)n);
//         params[0] = n_str;
//         num++;
//     }

//     params[0 + num] = mode;
//     params[1 + num] = address;
//     params[2 + num] = port_str;

//     SIM_error err = SIM_sendAT(sim, "CIPSTART", params);
//     if (err != SIM_ok)
//         return err;

// #ifdef SIM_CHECK_OK
//     err = SIM_receiveRaw(sim, 75000);
//     if (err != SIM_ok)
//         return err;

//     err = SIM_retrieveErr_find(sim);
//     if (err != SIM_ok)
//         return err;
// #endif

//     if (resp != NULL)
//     {
// #ifndef SIM_CHECK_OK
//         err = SIM_receiveRaw(sim, 75000);
//         if (err != SIM_ok)
//             return err;

//         err = SIM_retrieveErr_find(sim);
//         if (err != SIM_ok)
//             return err;
// #endif
//         SIM_err_pair c_err[] = {{.name = "CONNECT OK", .err = SIM_connectOk},
//                                 {.name = "ALREADY CONNECT", .err = SIM_alreadyConnect},
//                                 {.name = "CONNECT FAIL", .err = SIM_connectFail}};

//         err = SIM_retrieveCustomErr_find(sim, c_err);
//         if (err != SIM_connectOk)
//             return err;
//     }

//     return err;
// }

// /* AT+CIPSEND Send Data Through TCP or UDP Connection */
// SIM_error SIM_writeCIPSEND(SIM_int *sim, SIM_resp *resp, const char *data)
// {
//     SIM_error err = SIM_sendAT_short(sim, "CIPSEND");
//     if (err != SIM_ok)
//         return err;

// #ifdef SIM_CHECK_OK
//     // err = SIM_receiveRaw(sim, 1000);
//     // if (err != SIM_ok)
//     //    return err;

//     SIM_err_pair c_st[] = {{.name = ">", .err = SIM_ok},
//                            {.name = NULL, .err = SIM_ok}};

//     err = LL_SIM_receiveRaw_checkCustomErr(sim, 30000, SIM_retrieveCustomErr_find, c_st);
//     // err = SIM_retrieveCustomErr_find(sim, c_st);
//     if (err < 0)
//         return err;

//     LL_SIM_sendData(sim, data);

// //    err = SIM_retrieveErr_find(sim);
// //    if (err != SIM_ok)
// //       return err;
// #endif

//     //    if (resp != NULL)
//     //    {
//     // #ifndef SIM_CHECK_OK
//     //       err = SIM_receiveRaw(sim, 645000);
//     //       if (err != SIM_ok)
//     //          return err;

//     //       err = SIM_retrieveErr_find(sim);
//     //       if (err != SIM_ok)
//     //          return err;
//     // #endif
//     //       SIM_err_pair c_err[] = {{.name = "CONNECT OK", .err = SIM_connectOk},
//     //                              {.name = "ALREADY CONNECT", .err = SIM_alreadyConnect},
//     //                              {.name = "CONNECT FAIL", .err = SIM_connectFail},
//     //                              NULL};

//     //       err = SIM_retrieveCustomErr_find(sim, c_err);
//     //       if (err != SIM_connectOk)
//     //          return err;
//     //    }

//     return err;
// }
