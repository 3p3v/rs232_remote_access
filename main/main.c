#include <stdio.h>
#include <SIM_cmd.h>
#include <SIM.h>
#include <esp_log.h>

void app_main(void)
{
    SIM_int sim;
    LL_SIM_def(&sim);
    char arr[100] = {};
    sim.buf = arr;
    sim.buf_len = 100;
    LL_SIM_init(&sim);

    // for(int i = 0; i < 10; i++)
    // {
    //     SIM_sendAT_null(&sim);
    //     LL_SIM_delay(100);
    // }

    // SIM_receiveRaw(&sim);
    SIM_handshake(&sim, 10);
    ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));
    SIM_checkNetReg(&sim);
    ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));

    SIM_writeCSTT(&sim, "internet", NULL, NULL);
    ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));
    SIM_execCIICR(&sim);
    ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));

    SIM_resp resp;
    SIM_respNull(&resp);
    ESP_LOGI("SIM" ,"%i", SIM_execCIFSR(&sim, &resp));
    // ESP_LOGI("SIM" ,"%i, %s", resp.data_len, resp.data);
    SIM_writeCIPSTART(&sim, &resp, SIM_con_def, "TCP", "3p3v.pl", 2014);
    ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));
    SIM_writeCIPSEND(&sim, &resp, "test\r\n\032");

    // sim_write
    // ESP_LOGI("SIM" ,"%i", SIM_retrieveErr(&sim));
    // for(int i = 0; i < sim.rec_len; i++)
    //     printf("Letter: %i\r\n", (int)sim.buf[i]);

    printf("END\r\n");
}
 