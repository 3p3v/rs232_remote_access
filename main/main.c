#include <stdio.h>
#include <string.h>
#include <SIM.h>
#include <esp_log.h>
#include <SIM_TCPIP.h>
#include <SIM_common_cmds.h>
// #include <mqtt.h>
// #include <mqtt_pal.h>
// #include <mbedtls/>
// #include <net_sockets.c>
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>

SIM_intf *sim;
// QueueHandle_t con0_queue;
// QueueHandle_t con1_queue;

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
// #include "protocol_examples_common.h"
// #include "esp_netif.h"

// #include "lwip/err.h"
// #include "lwip/sockets.h"
// #include "lwip/sys.h"
// #include "lwip/netdb.h"
// #include "lwip/dns.h"

#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include "psa/crypto.h"
#endif
#include "esp_crt_bundle.h"


/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "www.google.com" //www.howsmyssl.com"
#define WEB_PORT "443"
#define WEB_URL "www.google.com" //https://www.howsmyssl.com/a/check"

static const char *TAG = "example";

static const char *REQUEST = "GET " WEB_URL " HTTP/1.0\r\n"
    "Host: "WEB_SERVER"\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
    "\r\n";


static void https_get_task()//void *pvParameters)
{
    // int take_break = 1;
    // while(take_break);
    
    char buf[512];
    int ret, flags, len;

    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_ssl_config conf;
    mbedtls_net_context server_fd;

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    psa_status_t status = psa_crypto_init();
    if (status != PSA_SUCCESS) {
        // ESP_LOGE(TAG, "Failed to initialize PSA crypto, returned %d", (int) status);
        return;
    }
#endif

    mbedtls_ssl_init(&ssl);
    mbedtls_x509_crt_init(&cacert);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    // ESP_LOGI(TAG, "Seeding the random number generator");

    mbedtls_ssl_config_init(&conf);

    mbedtls_entropy_init(&entropy);
    if((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                                    NULL, 0)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
        abort();
    }

    // ESP_LOGI(TAG, "Attaching the certificate bundle...");

    ret = esp_crt_bundle_attach(&conf);

    if(ret < 0)
    {
        // ESP_LOGE(TAG, "esp_crt_bundle_attach returned -0x%x", -ret);
        abort();
    }

    // ESP_LOGI(TAG, "Setting hostname for TLS session...");

     /* Hostname set here should match CN in server certificate */
    if((ret = mbedtls_ssl_set_hostname(&ssl, WEB_SERVER)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
        abort();
    }

    // ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");

    if((ret = mbedtls_ssl_config_defaults(&conf,
                                          MBEDTLS_SSL_IS_CLIENT,
                                          MBEDTLS_SSL_TRANSPORT_STREAM,
                                          MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
        // goto exit;
    }

    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE);//MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
#ifdef CONFIG_MBEDTLS_DEBUG
    mbedtls_esp_enable_debug_log(&conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
#endif

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
    mbedtls_ssl_conf_min_tls_version(&conf, MBEDTLS_SSL_VERSION_TLS1_3);
    mbedtls_ssl_conf_max_tls_version(&conf, MBEDTLS_SSL_VERSION_TLS1_3);
#endif
    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
    {
        // ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x", -ret);
        goto exit;
    }

    // while(1) {
        mbedtls_net_init(&server_fd);

        // ESP_LOGI(TAG, "Connecting to %s:%s...", WEB_SERVER, WEB_PORT);

        if ((ret = mbedtls_net_connect(&server_fd, WEB_SERVER,
                                      WEB_PORT, MBEDTLS_NET_PROTO_TCP)) != 0)
        {
            // ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
            goto exit;
        }

        // // ESP_LOGI(TAG, "Connected.");

        mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

        // ESP_LOGI(TAG, "Performing the SSL/TLS handshake...");

        while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
        {
            if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            {
                // ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
                goto exit;
            }
        }

        // ESP_LOGI(TAG, "Verifying peer X.509 certificate...");

        if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0)
        {
            /* In real life, we probably want to close connection if ret != 0 */
            ESP_LOGW(TAG, "Failed to verify peer certificate!");
            bzero(buf, sizeof(buf));
            mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", flags);
            ESP_LOGW(TAG, "verification info: %s", buf);
        }
        else {
            // ESP_LOGI(TAG, "Certificate verified.");
        }

        // ESP_LOGI(TAG, "Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&ssl));

        // ESP_LOGI(TAG, "Writing HTTP request...");

        size_t written_bytes = 0;
        do {
            ret = mbedtls_ssl_write(&ssl,
                                    (const unsigned char *)REQUEST + written_bytes,
                                    strlen(REQUEST) - written_bytes);
            if (ret >= 0) {
                // ESP_LOGI(TAG, "%d bytes written", ret);
                written_bytes += ret;
            } else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ) {
                // ESP_LOGE(TAG, "mbedtls_ssl_write returned -0x%x", -ret);
                goto exit;
            }
        } while(written_bytes < strlen(REQUEST));

        // ESP_LOGI(TAG, "Reading HTTP response...");

        do
        {
            len = sizeof(buf) - 1;
            bzero(buf, sizeof(buf));
            ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf, len);

#if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS
            // if (ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET) {
            //     ESP_LOGD(TAG, "got session ticket in TLS 1.3 connection, retry read");
            //     continue;
            // }
#endif // CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS

            if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                continue;
            }

            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
                ret = 0;
                break;
            }

            if (ret < 0) {
                // ESP_LOGE(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
                break;
            }

            if (ret == 0) {
                // ESP_LOGI(TAG, "connection closed");
                break;
            }

            len = ret;
            ESP_LOGD(TAG, "%d bytes read", len);
            /* Print response directly to stdout as it is read */
            for (int i = 0; i < len; i++) {
                putchar(buf[i]);
            }
        } while(1);

        mbedtls_ssl_close_notify(&ssl);

    exit:
        mbedtls_ssl_session_reset(&ssl);
        mbedtls_net_free(&server_fd);

        if (ret != 0) {
            mbedtls_strerror(ret, buf, 100);
            // ESP_LOGE(TAG, "Last error was: -0x%x - %s", -ret, buf);
        }

        putchar('\n'); // JSON output doesn't have a newline at end

        static int request_count;
        // ESP_LOGI(TAG, "Completed %d requests", ++request_count);
        printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

        for (int countdown = 10; countdown >= 0; countdown--) {
            // ESP_LOGI(TAG, "%d...", countdown);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        // ESP_LOGI(TAG, "Starting again!");
    // }
}

void app_main(void)
{
    struct timeval tv_now;
    struct timezone t_z = {.tz_dsttime = 2, .tz_minuteswest = -39};
    gettimeofday(&tv_now, NULL);
    tv_now.tv_sec = 1694784830;
    settimeofday(&tv_now, NULL);
    gettimeofday(&tv_now, NULL);
    printf("%lli", tv_now.tv_sec);

    SIM_intf sim_obj;
    sim = &sim_obj;
    LL_SIM_def(sim);
    printf("END1\r\n");
    unsigned char arr[12000] = {};
    sim->buf = arr;
    sim->buf_len = 12000;
    LL_SIM_init(sim);
    LL_SIM_listen(sim);

    SIM_error err;
    SIM_cmd cmd;

    err = SIM_run(sim, SIM_execATE0(&cmd));

    SIM_readCGATT(&cmd);
    err = SIM_run(sim, &cmd);

    SIM_writeCIPMUX(&cmd, 1);
    err = SIM_run(sim, &cmd);

    SIM_writeCSTT(&cmd, "internet", NULL, NULL);
    err = SIM_run(sim, &cmd);

    SIM_execCIICR(&cmd);
    err = SIM_run(sim, &cmd);

    SIM_execCIFSR(&cmd);
    err = SIM_run(sim, &cmd);

    printf("%i", err);
    
    ESP_ERROR_CHECK( nvs_flash_init() );
    // ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    //  * Read "Establishing Wi-Fi or Ethernet Connection" section in
    //  * examples/protocols/README.md for more information about this function.
    //  */
    // // ESP_ERROR_CHECK(example_connect());

    https_get_task();

    // while(1);
}

// void app_main(void)
// {
//     // con0_queue = xQueueCreate(10, sizeof(SIM_cmd *));
//     // con1_queue = xQueueCreate(10, sizeof(SIM_cmd *));
    

//     SIM_error err;
    
//     LL_SIM_def(sim);
//     printf("END1\r\n");
//     unsigned char arr[100] = {};
//     sim.buf = arr;
//     sim.buf_len = 100;
//     LL_SIM_init(sim);
//     LL_SIM_listen(sim);

//     SIM_cmd cmd;

//     SIM_readCGATT(&cmd);
//     err = SIM_run(sim, &cmd);

//     SIM_writeCIPMUX(&cmd, 1);
//     err = SIM_run(sim, &cmd);

//     SIM_writeCSTT(&cmd, "internet", NULL, NULL);
//     err = SIM_run(sim, &cmd);

//     SIM_execCIICR(&cmd);
//     err = SIM_run(sim, &cmd);

//     SIM_execCIFSR(&cmd);
//     err = SIM_run(sim, &cmd);

//     SIM_cmd cmd_tcp;
//     SIM_listenTCP(&cmd_tcp, SIM_con_0);
//     err = SIM_run_multiple_launch(sim, &cmd_tcp);

//     SIM_writeCIPSTART(&cmd, SIM_con_0, "TCP", "3p3v.pl", 2014);
//     err = SIM_run(sim, &cmd);

//     const char *data = "Test c-string.\r\n";
//     SIM_writeCIPSEND(&cmd, SIM_con_0, 0, data, strlen(data));
//     err = SIM_run(sim, &cmd);

//     printf("END\r\n");
// }
