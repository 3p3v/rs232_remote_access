#pragma once

#include "SIM_common_def.h"
#include "SIM_common.h"

// #define LL_SIM_DEF_BUF_SIZE SIM_DEF_BUF_SIZE
// #define LL_SIM_DEF_TX_BUF_SIZE SIM_DEF_TX_BUF_SIZE

typedef LL_SIM_error SIM_error;
typedef LL_SIM_pin SIM_pin;

// typedef LL_SIM_receiveRaw SIM_receiveRaw;
// typedef LL_SIM_retrieveErr SIM_retrieveErr;
// typedef LL_SIM_retriveResp SIM_retriveResp;
// typedef LL_SIM_retrieveData SIM_retrieveData;

SIM_error SIM_execAT(SIM_intf *sim);

/* AT+CSQ Signal Quality Report */
//  SIM_error SIM_execCSQ(int & rssi, int & ber) ;

/* AT+CCID Show ICCID */
SIM_error SIM_execCCID(const SIM_intf *sim, SIM_resp *resp);

/* AT+CREG Network Registration */
SIM_error SIM_readCREG(const SIM_intf *sim, SIM_resp *resp);

typedef enum SIM_CREG_stat
{
    SIM_CREG_stat_notRegMT = 0,
    SIM_CREG_stat_regHomeNet,
    SIM_CREG_stat_notReg,
    SIM_CREG_stat_regDenied,
    SIM_CREG_stat_unkonown,
    SIM_CREG_stat_regRoam
} SIM_CREG_stat;

/* AT+SAPBR Bearer Settings for Applications Based on IP */
SIM_error SIM_writeSAPBR(const SIM_intf *sim, SIM_resp *resp, const unsigned char cmd_type, const unsigned char cid, const char *ConParamTag, const char *ConParamValue);
// SIM_error SIM_writeSAPBR(const SIM_intf *sim, SIM_resp *resp, unsigned char cmd_type, unsigned char cid);

/* AT+HTTPINIT Initialize HTTP Service  */
SIM_error SIM_execHTTPINIT(const SIM_intf *sim, SIM_resp *resp);

/* AT+HTTPPARA Set HTTP Parameters Value */
SIM_error SIM_writeHTTPPARA(const SIM_intf *sim, SIM_resp *resp, const char *HTTPParamTag, const char *HTTPParamValue);

/* AT+HTTPDATA Input HTTP Data */
SIM_error SIM_writeHTTPDATA(const SIM_intf *sim, SIM_resp *resp, const int size, const int time, const char *inputData);

/*  AT+HTTPACTION HTTP Method Action
 *   method =:
 *   - 0 - GET,
 *   - 1 - POST,
 *   - 2 - HEAD.
 */
SIM_error SIM_writeHTTPACTION(const SIM_intf *sim, SIM_resp *resp, const unsigned char method);

/* AT+HTTPREAD Read the HTTP Server Response */
//  int SIM_writeHTTPREAD(const int start_address, const int byte_size, char * receivedData) ;
SIM_error SIM_execHTTPREAD(const SIM_intf *sim, SIM_resp *resp);

/* AT+HTTPTERM Terminate HTTP Service */
SIM_error SIM_execHTTPTERM(const SIM_intf *sim, SIM_resp *resp);

/* AT+CSCLK Configure Slow Clock */
SIM_error SIM_writeCSCLK(const SIM_intf *sim, SIM_resp *resp, const unsigned char n);

/* AT+CFUN Set Phone Functionality */
SIM_error SIM_writeCFUN(const SIM_intf *sim, SIM_resp *resp,const unsigned char fun);


/* AT+CPOWD Power off */
SIM_error SIM_writeCPOWD(const SIM_intf *sim, SIM_resp *resp, const unsigned char n);

/* AT+CPIN Enter PIN */
SIM_error SIM_readCPIN(const SIM_intf *sim, SIM_resp *resp);

/* AT+CSQ Signal Quality Report; */
/* returns "rrsi" and "ber" */
SIM_error SIM_execCSQ(const SIM_intf *sim, SIM_resp *resp);

/* AT+CGATT Attach or Detach from GPRS Service */
// SIM_error SIM_readCGATT(const SIM_intf *sim, SIM_resp *resp);

/* AT+CSTT Start Task and Set APN, USER NAME, PASSWORD */
SIM_error SIM_writeCSTT(const SIM_intf *sim, const char *apn, const char *username, const char *password);

/* AT+CIICR Bring Up Wireless Connection with GPRS or CSD */
SIM_error SIM_execCIICR(const SIM_intf *sim);

/* AT+CIFSR Get Local IP Address */
SIM_error SIM_execCIFSR(const SIM_intf *sim, SIM_resp *resp);

/* AT+CIPSTART Start Up TCP or UDP Connection */
SIM_error SIM_writeCIPSTART(SIM_intf *sim, SIM_resp *resp, const SIM_con_num n, char *mode, char *address, const unsigned int port);

/* AT+CIPSEND Send Data Through TCP or UDP Connection */
SIM_error SIM_writeCIPSEND(SIM_intf *sim, SIM_resp *resp, const char *data);