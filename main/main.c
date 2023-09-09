#include <stdio.h>
#include <string.h>
#include <SIM_cmd.h>
#include <SIM.h>
#include <esp_log.h>
#include <SIM_TCPIP.h>

void app_main(void)
{
    SIM_error err;
    SIM_intf sim;
    LL_SIM_def(&sim);
    printf("END1\r\n");
    char arr[100] = {};
    sim.buf = arr;
    sim.buf_len = 100;
    LL_SIM_init(&sim);
    LL_SIM_listen(&sim);

    SIM_cmd cmd;

    SIM_readCGATT(&cmd);
    err = SIM_run(&sim, &cmd);

    SIM_writeCSTT(&cmd, "internet", NULL, NULL);
    err = SIM_run(&sim, &cmd);

    SIM_execCIICR(&cmd);
    err = SIM_run(&sim, &cmd);

    SIM_execCIFSR(&cmd);
    err = SIM_run(&sim, &cmd);

    SIM_cmd cmd_tcp;
    SIM_listenTCP(&cmd_tcp, 0);
    err = SIM_run_multiple_launch(&sim, &cmd_tcp);

    SIM_writeCIPSTART(&cmd, SIM_con_def, "TCP", "3p3v.pl", 2014);
    err = SIM_run(&sim, &cmd);

    const char *data = "Test c-string.\r\n" ;
    SIM_execCIPSEND(&cmd, data, strlen(data));
    err = SIM_run(&sim, &cmd);

    // for(int i = 0; i < 10; i++)
    // {
    //     SIM_sendAT_null(&sim);
    //     LL_SIM_delay(100); 
    // }

    // SIM_receiveRaw(&sim);
    // SIM_handshake(&sim, 10);
    // ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));
    // SIM_checkNetReg(&sim);
    // ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));

    // SIM_writeCSTT(&sim, "internet", NULL, NULL);
    // ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));
    // SIM_execCIICR(&sim);
    // ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));

    // SIM_resp resp;
    // SIM_respNull(&resp);
    // ESP_LOGI("SIM" ,"%i", SIM_execCIFSR(&sim, &resp));
    // // ESP_LOGI("SIM" ,"%i, %s", resp.data_len, resp.data);
    // SIM_writeCIPSTART(&sim, &resp, SIM_con_def, "TCP", "3p3v.pl", 2014);
    // ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));
    // SIM_writeCIPSEND(&sim, &resp, "test\r\n\032");

    // sim_write
    // ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));
    // for(int i = 0; i < sim.rec_len; i++)
    //     printf("Letter: %i\r\n", (int)sim.buf[i]);

    printf("END\r\n");
}
 