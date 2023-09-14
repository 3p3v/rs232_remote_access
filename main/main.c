#include <stdio.h>
#include <string.h>
#include <SIM.h>
#include <esp_log.h>
#include <SIM_TCPIP.h>
// #include <mqtt.h>
// #include <mqtt_pal.h>
// #include <mbedtls/>
// #include <net_sockets.c>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>

SIM_intf sim;
QueueHandle_t con0_queue;
QueueHandle_t con1_queue;

void app_main(void)
{
    con0_queue = xQueueCreate(10, sizeof(SIM_cmd *));
    con1_queue = xQueueCreate(10, sizeof(SIM_cmd *));
    
    
    
    
    // mbedtls_net_init(NULL);
    int ret;
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;


    // mbedtls_ssl_set_bio(ssl, server_fd, )

    // mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    // esp_crt_bundle_attach(&conf);
    // mbedtls_x509_crt_init(&cacert);
    // mbedtls_ctr_drbg_init(&ctr_drbg);

    // mbedtls_entropy_init(&entropy);
    // if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
    //                                  (const unsigned char *)pers,
    //                                  strlen(pers))) != 0)
    // {
    //     printf(" failed\n  ! mbedtls_ctr_drbg_seed returned %d\n", ret);
    //     // goto exit;
    // }
    // mqtt_pal_sendall(0, NULL, 0, 0);

    SIM_error err;
    
    LL_SIM_def(&sim);
    printf("END1\r\n");
    unsigned char arr[100] = {};
    sim.buf = arr;
    sim.buf_len = 100;
    LL_SIM_init(&sim);
    LL_SIM_listen(&sim);

    SIM_cmd cmd;

    SIM_readCGATT(&cmd);
    err = SIM_run(&sim, &cmd);

    SIM_writeCIPMUX(&cmd, 1);
    err = SIM_run(&sim, &cmd);

    SIM_writeCSTT(&cmd, "internet", NULL, NULL);
    err = SIM_run(&sim, &cmd);

    SIM_execCIICR(&cmd);
    err = SIM_run(&sim, &cmd);

    SIM_execCIFSR(&cmd);
    err = SIM_run(&sim, &cmd);

    SIM_cmd cmd_tcp;
    SIM_listenTCP(&cmd_tcp, SIM_con_0);
    err = SIM_run_multiple_launch(&sim, &cmd_tcp);

    SIM_writeCIPSTART(&cmd, SIM_con_0, "TCP", "3p3v.pl", 2014);
    err = SIM_run(&sim, &cmd);

    const char *data = "Test c-string.\r\n";
    SIM_writeCIPSEND(&cmd, SIM_con_0, 0, data, strlen(data));
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
