#pragma once

#include "SIM_common.h"

// #define SIM_TCPIP

/* AT+CGATT Attach or Detach from GPRS Service */
#define SIM_READCGATT_TIMEOUT 100
/* AT+CGATT Attach or Detach from GPRS Service */
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
#define SIM_WRITECIPCLOSE_TIMEOUT 5000
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
SIM_error SIM_listenTCP(SIM_intf *sim, const SIM_con_num n, void (*resp_handler)(SIM_error *));
SIM_error SIM_listenTCP_setHandler(SIM_intf *sim, const SIM_con_num n,  void (*resp_handler)(SIM_error *));
SIM_error SIM_listenTCP_free(SIM_intf *sim, const SIM_con_num n);
/* AT+CIPMUX Start Up Multi-IP Connection */
#define SIM_WRITECIPMUX_TIMEOUT 100
SIM_cmd *SIM_writeCIPMUX(SIM_cmd *cmd, const unsigned char n);

