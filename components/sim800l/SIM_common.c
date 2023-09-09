#include <string.h>
#include "SIM_common.h"
#include <esp_log.h>

static void SIM_util_atcpy(char *at, const char *type)
{
   size_t type_last = strlen(type) - 1;

   if (type[type_last] == '?')
   {
      memcpy(at, type, type_last);
      at[type_last] = '\0';
   }
   else
   {
      strcpy(at, type);
   }
}

static char *SIM_util_strchar(const SIM_resp *resp, const char *start, const char find)
{
   for (int i = (start - resp->resp); i < resp->resp_len - 2; i++) // - 2 loops, becaurse of endl
   {
      if (*(resp->resp + i) == find)
         return (resp->resp + i);
   }

   return NULL;

   // if ((start + len) == ',')
   //    return ',';
   // else if ((start + len) == '\r')
   //    return '\r';
   // else
   //    return NULL;
}

SIM_error SIM_run(SIM_intf *sim, SIM_cmd *cmd)
{
   SIM_error err = SIM_ok;
   xSemaphoreTake(sim->add_cmd_mutex, portMAX_DELAY);
   // Send command
   err = SIM_sendAT(sim, cmd->at, cmd->params);
   if (err != SIM_ok)
      goto REGION_END;

   // Give handlers to the listener and wait for the end of en execution
   err = SIM_sub(sim, cmd);

   REGION_END:
   xSemaphoreGive(sim->add_cmd_mutex);
   return err;
}

SIM_error SIM_run_multiple_launch(SIM_intf *sim, SIM_cmd *cmd)
{
   SIM_error err = SIM_ok;

   // Give handlers to the listener and wait for the end of en execution
   return SIM_sub_multiple_launch(sim, cmd);
}

SIM_error SIM_sub(SIM_intf *sim, SIM_cmd *cmd)
{
   SIM_error err;

   sim->cmds[sim->cmds_num].cmd = cmd;
   sim->cmds_num++;

   err = LL_SIM_wait(sim, cmd->timeout);

   // 'delete' cmd from listener
   sim->cmds_num--;
   sim->cmds[sim->cmds_num].cmd = NULL;

   return err;
}

SIM_error SIM_sub_multiple_launch(SIM_intf *sim, SIM_cmd *cmd)
{
   sim->cmds[sim->cmds_num].cmd = cmd;
   sim->cmds_num++;

   return SIM_ok;
}

// SIM_error SIM_check()

// SIM_error SIM_watch()
// {
   
// }

/* Send AT command with any params.
 * Given array of params must contain
 * a NULL ptr after the last parameter.
 */
SIM_error SIM_sendAT(const SIM_intf *sim, const char *type, const char params[SIM_MAX_PARAMS][SIM_MAX_PARAM_LEN]) // TODO check length
{
   const char *beg = "AT+";
   const char *end = "=";
   const char *new_line = "\r\n";

   unsigned int len = strlen(type) + strlen(beg) + strlen(end);

   int p_num;

   for (p_num = 0; p_num < SIM_MAX_PARAMS; p_num++)
   {
      if (params[p_num][0] == '\0')
         break;

      len += strlen(*(params + p_num)) + 1;
   }
   len--;

   // char temp[len + strlen(new_line) + 1] = {};
   char *temp = sim->buf;
   memcpy(temp, beg, strlen(beg));
   int len2 = strlen(beg);

   memcpy(temp + len2, type, strlen(type));
   len2 += strlen(type);

   if (p_num > 0)
   {
      memcpy(temp + len2, end, strlen(end));
      len2 += strlen(end);
   }

   for (int i = 0; i < SIM_MAX_PARAMS; i++)
   {
      if (*(*(params + i) + 0) == '\0')
         break;

      memcpy(temp + len2, *(params + i), strlen(*(params + i)));
      len2 += strlen(*(params + i));
      if (len2 < len)
      {
         *(temp + len2) = ',';
         len2 += 1;
      }
   }

   memcpy(temp + len2, new_line, strlen(new_line));
   len2 += strlen(new_line);

   temp[len2] = '\0';

   return LL_SIM_sendData(sim, temp);
}

// SIM_error SIM_sendAT_short(const SIM_intf *sim, const char *type)
// {
//    const char *beg = "AT+";
//    const char *new_line = "\r\n";

//    // int len = strlen(type) + strlen(beg) + strlen(new_line);
//    // char temp[len + 1] = {};
//    char *temp = sim->buf;

//    memcpy(temp, beg, strlen(beg));
//    int len2 = strlen(beg);

//    memcpy(temp + len2, type, strlen(type));
//    len2 += strlen(type);

//    memcpy(temp + len2, new_line, strlen(new_line));
//    len2 += strlen(new_line);

//    temp[len2] = '\0';

//    return LL_SIM_sendData(sim, temp);
// }

// SIM_error SIM_sendAT_null(const SIM_intf *sim)
// {
//    return LL_SIM_sendData(sim, "AT\r\n");
// }

// SIM_error SIM_receiveRaw(SIM_intf *sim, SIM_time time)
// {
//    // Download answer
//    SIM_data_len err = LL_SIM_receiveRaw(sim, time);
//    // Secure string against out of bounds exceptions
//    if (err >= 0)
//    {
//       sim->rec_len = err;
//       *(sim->buf + sim->buf_len) = '\0';

//       *(sim->buf + sim->rec_len) = '\0';

//       return SIM_ok;
//    }

//    sim->buf_len = 0;
//    return err;
// }

/* Get error code, error code must be located at the end of the message */
SIM_errMsgEnd_pair SIM_retrieveErr(char *buf, unsigned int rec_len) // TODO fix
{
   SIM_errMsgEnd_pair err;
   
   err.ptr = strstr(buf + rec_len - 4, "\r\nOK\r\n");
   if (rec_len >= 4 && err.ptr)
   {
      err.err = SIM_ok;
      err.ptr += strlen("\r\nOK\r\n");
      return err;
   }

   err.ptr = strstr(buf + rec_len - 7, "\r\nERROR\r\n");
   if (rec_len >= 7 && err.ptr)
   {
      err.err = SIM_err;
      err.ptr += strlen("\r\nERROR\r\n");
      return err;
   }

   err.err = SIM_noErrCode;
   return err;
}

/* Get error code, can be located anywhere, starts searching from the top */
SIM_errMsgEnd_pair SIM_retrieveErr_find(char *buf, unsigned int rec_len) // TODO fix
{
   SIM_errMsgEnd_pair err;
   
   err.ptr = strstr(buf, "\r\nOK\r\n");
   if (rec_len >= 4 && err.ptr)
   {
      err.err = SIM_ok;
      err.ptr += strlen("\r\nOK\r\n");
      return err;
   }

   err.ptr = strstr(buf, "\r\nERROR\r\n");
   if (rec_len >= 7 && err.ptr)
   {
      err.err = SIM_err;
      err.ptr += strlen("\r\nERROR\r\n");
      return err;
   }

   err.err = SIM_noErrCode;
   return err;
}

/* Find custom error message, error code must be located at the end of the message */
SIM_errMsgEnd_pair SIM_retrieveCustomErr_find(char *buf, unsigned int rec_len, const SIM_err_pair *err)
{
   SIM_errMsgEnd_pair err_pair;
   int i = 0;

   for(;;)
   {
      if (err[i].name == NULL)
      {
         err_pair.err = err[i].err;
         err_pair.ptr = NULL;
         return err_pair;
      }
      else
      {
         err_pair.ptr = strstr(buf, err[i].name);
         if (err_pair.ptr != NULL)
         {
            err_pair.err = err[i].err;
            err_pair.ptr += strlen(err[i].name);
            return err_pair;
         }
      }
      
      i++;
   }
}

/* Find custom error message, can be located anywhere, starts searching from the top */
// SIM_error SIM_retrieveCustomErr_find(const SIM_intf *sim, const SIM_err_pair *err)
// {
//    int i = 0;

//    for(;;)
//    {
//       if ((err + i)->name == NULL)
//       {
//          return SIM_noResp;
//       }
//       else if (strstr(sim->buf, err[i].name))
//       {
//          return err[i].err;
//       }

//       i++;
//    }
// }

/* Get ptr to response, returns length of the response */
// SIM_error SIM_retrieveCustomResp(const SIM_intf *sim, SIM_resp *resp, const char *resp_name)
// {
//    // Find line starting with "+"
//    char ret[SIM_MAX_AT_STR_LEN + 2] = {};
//    strcat(ret, resp_name);
//    char *start = strstr((sim->buf + 0), ret); //const ???

//    if (start == NULL)
//    {
//       resp->resp_name = NULL;
//       resp->resp_name_len = 0;
//       resp->resp = NULL;
//       resp->resp_len = 0;
//       return SIM_noResp;
//    }
//    else
//    {
//       resp->resp_name = start;
//       resp->resp_name_len = strlen(resp_name);
//       start += strlen(ret) + 1;
//    }

//    // Find endl
//    const char *endl = strstr((start + 0), "\r\n");

//    if (start < endl) //endl && start < endl
//    {
//       resp->resp_len = (endl - start) + 2; // + 2 to add endl
//       resp->resp = start;
//       return SIM_ok;
//    }
//    else
//    {
//       resp->resp_name = NULL;
//       resp->resp_name_len = 0;
//       resp->resp_len = 0;
//       resp->resp = NULL;
//       if (start && !endl)
//          return SIM_bufferFullErr;
//       else
//          return SIM_recErr;
//    }
// }

/* Get ptr to response, returns length of the response */
SIM_error SIM_retrieveResp(char *buf, unsigned int rec_len, SIM_resp *resp)
{
   // Find line starting with "+"
   char ret[SIM_MAX_AT_STR_LEN + 2] = {};
   ret[0] = '+';
   strcat(ret, resp->at);
   strcat(ret, ":");
   char *start = strstr((buf + 0), ret); // const ???

   if (start == NULL)
   {
      resp->resp = NULL;
      resp->resp_len = 0;
      return SIM_noResp;
   }
   else
   {
      start += strlen(ret) + 1;
   }

   // Find endl
   const char *endl = strstr((start + 0), "\r\n");

   if (start < endl) // endl && start < endl
   {
      resp->resp_len = (endl - start); // + 2; // + 2 to add endl
      resp->resp = start;
      return SIM_ok;
   }
   else
   {
      resp->resp_len = 0;
      resp->resp = NULL;
      if (start && !endl)
         return SIM_bufferFullErr;
      else
         return SIM_recErr;
   }
}

SIM_data_len SIM_retrieveParams(SIM_resp *resp)
{
   resp->params[0].ptr = resp->resp;

   if (resp->params[0].ptr == NULL || resp->resp_len == 0)
   {
      return SIM_recErr;
   }

   char *end = SIM_util_strchar(resp, resp->params[0].ptr, ',');

   if (end == NULL)
   {
      resp->params[0].len = resp->resp_len; // - 2;
      resp->params_num = 1;
      return SIM_ok;
   }

   resp->params[0].len = (end - resp->params[0].ptr);

   for (int i = 1; i < SIM_MAX_PARAMS; i++)
   {
      resp->params[i].ptr = resp->params[i - 1].ptr + resp->params[i - 1].len;

      if (*(resp->params[i].ptr++) != ',')
         return SIM_recErr;

      end = SIM_util_strchar(resp, resp->params[i].ptr, ',');

      if (end != NULL)
      {
         resp->params[i].len = (end - resp->params[i].ptr);
      }
      else if (end == NULL)
      {
         resp->params[i].len = (resp->resp + resp->resp_len) - resp->params[i].ptr; // - 2;
         resp->params_num = i + 1;
         return SIM_ok;
      }
   }

   resp->params_num = SIM_MAX_PARAMS;
   return SIM_ok;
}

/* Get ptr to parameter, first parameter is "param_num = 0",
 * returns length of parameter
 */
// SIM_data_len SIM_retrieveParam(const char *resp, SIM_data_len resp_len, const unsigned char param_num, char *out)
// {
//    if (param_num == 0)
//    {
//       out = strstr(resp, ":");
//       if (out == NULL || ++out > (resp + resp_len))
//       {
//          out = NULL;
//          return SIM_recErr;
//       }

//       const char *end = strstr(out, ",");
//       if (end > (resp + resp_len))
//       {
//          out = NULL;
//          return SIM_recErr;
//       }
//       else if (end == NULL)
//       {
//          end = strstr(out, "\r\n");
//          if (end == NULL || end > (resp + resp_len))
//          {
//             out = NULL;
//             return SIM_recErr;
//          }
//       }
//       else
//       {
//          return (SIM_data_len)(end - out);
//       }
//    }
//    else
//    {
//       for (int i = 0; i <= param_num; i++)
//       {
//          out = strstr(resp, ",");
//          if (out == NULL || ++out > (resp + resp_len))
//             return SIM_recErr;
//       }

//       const char *end = strstr(out, ",");
//       if (end > (resp + resp_len))
//       {
//          out = NULL;
//          return SIM_recErr;
//       }
//       else if (end == NULL)
//       {
//          end = strstr(out, "\r\n");
//          if (end == NULL || end > (resp + resp_len))
//          {
//             out = NULL;
//             return SIM_recErr;
//          }
//       }
//       else
//       {
//          return (SIM_data_len)(end - out);
//       }
//    }
// }

/* Get ptr to data part of the response, returs length of data */
SIM_data_len SIM_retrieveData(char *buf, unsigned int rec_len, SIM_resp *resp, const unsigned int data_len) // TODO
{

   resp->data = strstr(buf, "\r\n") + 4;

   if (resp->data + resp->data_len > buf + rec_len)
   {
      resp->data = NULL;
      resp->data_len = 0;
      return SIM_recErr;
   }
   else
   {
      if (data_len != 0)
      {
         resp->data_len = data_len;
         return resp->data_len;
      }
      else
      {
         resp->data_len = rec_len - (resp->data - buf);

         if (rec_len >= 4 && strstr(buf, "\r\nOK\r\n"))
         {
            resp->data_len -= 6;
         }

         return resp->data_len;
      }
   }
}

// SIM_error SIM_receive(SIM_intf *sim, SIM_time time)
// {
//    SIM_error err = SIM_receiveRaw(sim, time);
//    if (err != SIM_ok)
//       return err;

//    err = SIM_retrieveErr(sim);

//    return err;
// }

SIM_error SIM_retrieve(char *buf, unsigned int rec_len, SIM_resp *resp)
{
   SIM_error err = SIM_retrieveResp(buf, rec_len, resp);
   ESP_LOGI("SIM", "SIM_retrieveResp: %i", err);
   if (err != SIM_ok)
      return err;

   err = SIM_retrieveParams(resp);

   ESP_LOGI("SIM", "%i", resp->params_num);

   return err;
}

// SIM_error LL_SIM_sendReceiveRetriveData(const char *data)
// {
//    SIM_error err = sendData(data);
//    if (err != Sim800lOk)
//       return err;

//    int errInt = receiveData();
//    if (errInt <= 0)
//       return (SIM_error)errInt;

//    err = retrieveErr();

//    ESP_LOGI("SIM", "%i, %i, %s", err, receivedLen, receivedData);

//    if (err == Sim800lRecErr)
//       return err;
//    else if (err == Sim800lOk)
//       return err;
//    else
//    {
//       errInt = receiveData();
//       if (errInt <= 0)
//          return (SIM_error)errInt;

//       err = retrieveErr();

//       return err;
//    }
// }

// void LL_SIM_simDelay(int ms) {}
