#include "SIM.h"
// #include "Sim800l.hpp"
#include "string.h"
#include "stdio.h"
#include <esp_log.h>

SIM_error SIM_execAT(SIM_int *sim)
{
   SIM_error err = SIM_sendAT_null(sim);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 100);
   if (err != SIM_ok)
      return err;
#endif

   return err;
}

SIM_error SIM_execCCID(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "CCID");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_noResp)
         return err;

      err = SIM_retrieveData(sim, resp, 0);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

SIM_error SIM_readCREG(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "CREG?");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 1000);
   if (err != SIM_ok)
      return err;

   // ESP_LOGI("", "");
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 1000);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+SAPBR Bearer Settings for Applications Based on IP */
SIM_error SIM_writeSAPBR(const SIM_int *sim, SIM_resp *resp, const unsigned char cmd_type, const unsigned char cid, const char *ConParamTag, const char *ConParamValue)
{
   SIM_error err = SIM_ok;

   const char *type = "SAPBR";
   char cmd_type_str[6] = {};
   char cid_str[6] = {};

   sprintf(cmd_type_str, "%u", (unsigned int)cmd_type);
   sprintf(cid_str, "%u", (unsigned int)cid);

   if (ConParamTag != NULL || ConParamValue != NULL)
   {
      const char *params[] = {cmd_type_str, cid_str, ConParamTag, ConParamValue, NULL};
      err = SIM_sendAT(sim, type, params);
      if (err != SIM_ok)
         return err;
   }
   else
   {
      const char *params[] = {cmd_type_str, cid_str, NULL};
      err = SIM_sendAT(sim, type, params);
      if (err != SIM_ok)
         return err;
   }

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 1850);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 1850);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

// SIM_error SIM_writeSAPBR(unsigned char cmd_type, unsigned char cid)
// {
//    return writeSAPBR(cmd_type, cid, nullptr, nullptr);
// }

/* AT+HTTPINIT Initialize HTTP Service  */
SIM_error SIM_execHTTPINIT(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "HTTPINIT");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+HTTPPARA Set HTTP Parameters Value */
SIM_error SIM_writeHTTPPARA(const SIM_int *sim, SIM_resp *resp, const char *HTTPParamTag, const char *HTTPParamValue)
{
   const char *params[] = {HTTPParamTag, HTTPParamValue, NULL};

   SIM_error err = SIM_sendAT(sim, "HTTPPARA", params);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+HTTPDATA Input HTTP Data */ // TODO
SIM_error SIM_writeHTTPDATA(const SIM_int *sim, SIM_resp *resp, const int size, const int time, const char *inputData)
{
   char size_str[6] = {};
   char time_str[6] = {};

   sprintf(size_str, "%u", (unsigned int)size);
   sprintf(time_str, "%u", (unsigned int)time);

   const char *params[] = {size_str, time_str, NULL};

   SIM_error err = SIM_sendAT(sim, "HTTPDATA", params);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   LL_SIM_delay(100);

   err = LL_SIM_sendData(sim, inputData);

   if (err != SIM_ok)
      return err;

   // ESP_LOGI("SIM", "TEST2");
   // if (receiveData() <= 0) //TODO check if needed
   //    return Sim800lErr;

   // ESP_LOGI("SIM", "TEST3");
   return err;
}

/*  AT+HTTPACTION HTTP Method Action
 *   method =:
 *   - 0 - GET,
 *   - 1 - POST,
 *   - 2 - HEAD.
 */
SIM_error SIM_writeHTTPACTION(const SIM_int *sim, SIM_resp *resp, const unsigned char method)
{
   char method_str[6] = {};

   sprintf(method_str, "%u", (unsigned int)method);

   const char *params[] = {method_str, NULL};

   SIM_error err = SIM_sendAT(sim, "HTTPACTION", params);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+HTTPREAD Read the HTTP Server Response */
SIM_error SIM_execHTTPREAD(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "HTTPREAD");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;

      if (resp->params_num != 1)
         return SIM_recErr;

      unsigned int data_len = SIM_atoi_uint32_t(resp->params[1].ptr, resp->params[1].len);

      err = SIM_retrieveData(sim, resp, data_len);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+HTTPTERM Terminate HTTP Service */
SIM_error SIM_execHTTPTERM(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "HTTPTERM");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

SIM_error SIM_writeCSCLK(const SIM_int *sim, SIM_resp *resp, const unsigned char n)
{
   char nStr[10] = {};

   sprintf(nStr, "%u", (unsigned int)n);

   const char *params[] = {nStr, NULL};

   SIM_error err = SIM_sendAT(sim, "CSCLK", params);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

SIM_error SIM_writeCFUN(const SIM_int *sim, SIM_resp *resp, const unsigned char fun)
{
   char funStr[10] = {};

   sprintf(funStr, "%u", (unsigned int)fun);

   const char *params[] = {funStr, NULL};

   SIM_error err = SIM_sendAT(sim, "CFUN", params);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

// SIM_error LL_SIM_setDRT(Sim800lPin set)
// {
//    return SIM_error();
// }

// SIM_error LL_SIM_setRST(Sim800lPin set)
// {
//    return SIM_error();
// }

SIM_error SIM_writeCPOWD(const SIM_int *sim, SIM_resp *resp, const unsigned char n)
{
   char nStr[10] = {};

   sprintf(nStr, "%u", (unsigned int)n);

   const char *params[] = {nStr, NULL};

   SIM_error err = SIM_sendAT(sim, "CPOWD", params);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 10);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+CPIN Enter PIN */
SIM_error SIM_readCPIN(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "CPIN?");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 100);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 100);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+CSQ Signal Quality Report; */
/* returns "rrsi" and "ber" */
SIM_error SIM_execCSQ(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "CSQ");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 100);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 100);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+CGATT Attach or Detach from GPRS Service */
SIM_error SIM_readCGATT(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "CGATT?");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 100);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 100);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+CSTT Start Task and Set APN, USER NAME, PASSWORD */
SIM_error SIM_writeCSTT(const SIM_int *sim, const char *apn, const char *username, const char *password)
{
   char *params[] = {apn, NULL, NULL};
   
   if (username && password)
   {
      params[1] = username;
      params[2] = password;
   }

   SIM_error err = SIM_sendAT(sim, "CSTT", params);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 1000);
   if (err != SIM_ok)
      return err;
#endif

   return err;
}

/* AT+CIICR Bring Up Wireless Connection with GPRS or CSD */
SIM_error SIM_execCIICR(const SIM_int *sim)
{
   SIM_error err = SIM_sendAT_short(sim, "CIICR");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 10000);
   if (err != SIM_ok)
      return err;
#endif

   return err;
}

/* AT+CIFSR Get Local IP Address */
SIM_error SIM_execCIFSR(const SIM_int *sim, SIM_resp *resp)
{
   SIM_error err = SIM_sendAT_short(sim, "CIFSR");
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receive(sim, 85000);
   if (err != SIM_noErrCode)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receive(sim, 85000);
      if (err != SIM_ok)
         return err;
#endif

      err = SIM_retrieve(sim, resp);
      if (err != SIM_noResp)
         return err;

      err = SIM_retrieveData(sim, resp, 0);
      if (err != SIM_ok)
         return err;
   }

   return err;
}

/* AT+CIPSTART Start Up TCP or UDP Connection */
SIM_error SIM_writeCIPSTART(SIM_int *sim, SIM_resp *resp, const SIM_con_num n, char *mode, char *address, const unsigned int port)
{
   char n_str[6] = {};
   char port_str[6] = {};
   sprintf(port_str, "%u", (unsigned int)port);
   char *params[5] = {NULL, NULL, NULL, NULL, NULL};
   unsigned char num = 0;

   if (n != SIM_con_def)
   {
      sprintf(n_str, "%i", (int)n);
      params[0] = n_str;
      num++;
   }

   params[0 + num] = mode;
   params[1 + num] = address;
   params[2 + num] = port_str;
   
   SIM_error err = SIM_sendAT(sim, "CIPSTART", params);
   if (err != SIM_ok)
      return err;

#ifdef SIM_CHECK_OK
   err = SIM_receiveRaw(sim, 75000);
   if (err != SIM_ok)
      return err;

   err = SIM_retrieveErr_find(sim);
   if (err != SIM_ok)
      return err;
#endif

   if (resp != NULL)
   {
#ifndef SIM_CHECK_OK
      err = SIM_receiveRaw(sim, 75000);
      if (err != SIM_ok)
         return err;

      err = SIM_retrieveErr_find(sim);
      if (err != SIM_ok)
         return err;
#endif
      SIM_err_map c_err[] = {{.name = "CONNECT OK", .err = SIM_connectOk},
                             {.name = "ALREADY CONNECT", .err = SIM_alreadyConnect},
                             {.name = "CONNECT FAIL", .err = SIM_connectFail}};

      err = SIM_retrieveCustomErr_find(sim, c_err);
      if (err != SIM_connectOk)
         return err;
   }

   return err;
}

