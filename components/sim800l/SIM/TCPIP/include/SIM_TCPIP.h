#pragma once

#include "SIM_common.h"

/* AT+CGATT Attach or Detach from GPRS Service */
#define SIM_READCGATT_TIMEOUT 100

void SIM_readCGATT(SIM_cmd *cmd);

SIM_error SIM_readCGATT_handler(char *buf, unsigned int rec_len, SIM_resp *resp);

// /* AT+CSTT Start Task and Set APN, USER NAME, PASSWORD */
// SIM_error SIM_writeCSTT(const SIM_int *sim, const char *apn, const char *username, const char *password)
// {
//     char *params[] = {apn, NULL, NULL};

//     if (username && password)
//     {
//         params[1] = username;
//         params[2] = password;
//     }

//     SIM_error err = SIM_sendAT(sim, "CSTT", params);
//     if (err != SIM_ok)
//         return err;

// #ifdef SIM_CHECK_OK
//     err = SIM_receive(sim, 1000);
//     if (err != SIM_ok)
//         return err;
// #endif

//     return err;
// }

// /* AT+CIICR Bring Up Wireless Connection with GPRS or CSD */
// SIM_error SIM_execCIICR(const SIM_int *sim)
// {
//     SIM_error err = SIM_sendAT_short(sim, "CIICR");
//     if (err != SIM_ok)
//         return err;

// #ifdef SIM_CHECK_OK
//     err = SIM_receive(sim, 10000);
//     if (err != SIM_ok)
//         return err;
// #endif

//     return err;
// }

// /* AT+CIFSR Get Local IP Address */
// SIM_error SIM_execCIFSR(const SIM_int *sim, SIM_resp *resp)
// {
//     SIM_error err = SIM_sendAT_short(sim, "CIFSR");
//     if (err != SIM_ok)
//         return err;

// #ifdef SIM_CHECK_OK
//     err = SIM_receive(sim, 85000);
//     if (err != SIM_noErrCode)
//         return err;
// #endif

//     if (resp != NULL)
//     {
// #ifndef SIM_CHECK_OK
//         err = SIM_receive(sim, 85000);
//         if (err != SIM_ok)
//             return err;
// #endif

//         err = SIM_retrieve(sim, resp); // TODO
//         if (err != SIM_noResp)
//             return err;

//         err = SIM_retrieveData(sim, resp, 0);
//         if (err > 0)
//             return err;
//         else
//             return SIM_ok;
//     }

//     return err;
// }

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
