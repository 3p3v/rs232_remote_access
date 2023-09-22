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

#include <mbedtls_sockets.h>
#include "mbedtls/platform.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/esp_debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include <mqtt.h>

#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include "psa/crypto.h"
#endif
#include "esp_crt_bundle.h"


/* Constants that aren't configurable in menuconfig */
#define WEB_SERVER "www.3p3v.pl" //www.howsmyssl.com"
#define WEB_PORT "8883"
#define SERVER_IM2_CERT   \
"-----BEGIN CERTIFICATE-----\n"\
"MIIEzTCCA7WgAwIBAgIRANBLb+XdW9Ih58dM9kaLMUYwDQYJKoZIhvcNAQELBQAw\n"\
"fjELMAkGA1UEBhMCUEwxIjAgBgNVBAoTGVVuaXpldG8gVGVjaG5vbG9naWVzIFMu\n"\
"QS4xJzAlBgNVBAsTHkNlcnR1bSBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eTEiMCAG\n"\
"A1UEAxMZQ2VydHVtIFRydXN0ZWQgTmV0d29yayBDQTAeFw0xNDA5MTExMjAwMDBa\n"\
"Fw0yNzA2MDkxMDQ2MzlaMIGDMQswCQYDVQQGEwJQTDEiMCAGA1UEChMZVW5pemV0\n"\
"byBUZWNobm9sb2dpZXMgUy5BLjEnMCUGA1UECxMeQ2VydHVtIENlcnRpZmljYXRp\n"\
"b24gQXV0aG9yaXR5MScwJQYDVQQDEx5DZXJ0dW0gR2xvYmFsIFNlcnZpY2VzIENB\n"\
"IFNIQTIwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDHhvLXeA4kyBVY\n"\
"Dr12JusjW+/HPPWpBmWnSZC9gadbNXCD/GlHrGruRWrKn0YBasbonXi5FLRon717\n"\
"aCCDt2+7URnJx4mLGt3X73yH3I+T+COuyTFOuLId68jYV4vfBm1N/N1/KohGr8+R\n"\
"7eT4f4agfXVQ+gDZT3pGTywqc9IId4uHaFjW+mr5vVfu7WjL8A20jOawvlqV2K4/\n"\
"qSrjPZsqHlzf46LHgbQEK1EjTPsFrfvAQxvGpHiEIAFsS+1d2xPIEJVIxvq+KyGb\n"\
"W/0SJIj7/SfYo+ItvJ2a4/G2JfoPF9wTCC0N8U5I/KXxjYYXBcIE9pQ16T24T+dr\n"\
"MfPG2mYtAgMBAAGjggE+MIIBOjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBRU\n"\
"md2b/+inDqMZnVu+QlffMPyPMjAfBgNVHSMEGDAWgBQIds3LB/8k9sXN7buQvOKE\n"\
"N0Z19zAOBgNVHQ8BAf8EBAMCAQYwLwYDVR0fBCgwJjAkoCKgIIYeaHR0cDovL2Ny\n"\
"bC5jZXJ0dW0ucGwvY3RuY2EuY3JsMGsGCCsGAQUFBwEBBF8wXTAoBggrBgEFBQcw\n"\
"AYYcaHR0cDovL3N1YmNhLm9jc3AtY2VydHVtLmNvbTAxBggrBgEFBQcwAoYlaHR0\n"\
"cDovL3JlcG9zaXRvcnkuY2VydHVtLnBsL2N0bmNhLmNlcjA5BgNVHSAEMjAwMC4G\n"\
"BFUdIAAwJjAkBggrBgEFBQcCARYYaHR0cDovL3d3dy5jZXJ0dW0ucGwvQ1BTMA0G\n"\
"CSqGSIb3DQEBCwUAA4IBAQDRhC+yVHfF/IFcNNKbkxX1aVsCZ0fa2t+MxyFcs7KO\n"\
"qW1fGpOOycughOFnbM+lz4Y3gt5RaOFJTm7YVUZZ3751s5tv8nhXeXfrRIpQN7Cu\n"\
"+Nei457HlDxEUItPlkYnDbdDes/96T19cICd1TmIPekYRXiyuPW4Wgx6vykmk91x\n"\
"LkJ0y74TzVtUofVF446qXvea95zNpzYCVMg+AOX3ZZyy9XfST6g4um+cw/Idv31d\n"\
"bnJdBzMOgHH3uw2YMiZQgDqvNRE+wAs+PTFEIKHmBc/t1n3Shvg9e68M+5ZRM8bE\n"\
"WGqgLqfreTgCsCQcv9MDYw9TFUbS17RdE6NtiPfszTky\n"\
"-----END CERTIFICATE-----"

#define SERVER_IM1_CERT   \
"-----BEGIN CERTIFICATE-----\n"\
"MIIEWDCCA0CgAwIBAgIPMR1+3eSSAgi72F73Jiu6MA0GCSqGSIb3DQEBCwUAMIGD\n"\
"MQswCQYDVQQGEwJQTDEiMCAGA1UEChMZVW5pemV0byBUZWNobm9sb2dpZXMgUy5B\n"\
"LjEnMCUGA1UECxMeQ2VydHVtIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MScwJQYD\n"\
"VQQDEx5DZXJ0dW0gR2xvYmFsIFNlcnZpY2VzIENBIFNIQTIwHhcNMTYwNjA4MDgw\n"\
"MTI5WhcNMjYwNjA2MDgwMTI5WjA9MQswCQYDVQQGEwJQTDEVMBMGA1UECgwMaG9t\n"\
"ZS5wbCBTLkEuMRcwFQYDVQQDDA5DZXJ0eWZpa2F0IFNTTDCCASIwDQYJKoZIhvcN\n"\
"AQEBBQADggEPADCCAQoCggEBALDozq2WteNT2CsyO5CY9rCN4neWGyj+26XUYS5o\n"\
"oM+tzFlMrbdNk3J+aI7P8Ab29qlSuPRgpHMi++AopeaOYsiwuVJZqnN2W1PC6huf\n"\
"b9mVMv3mcxYLCmuEiY4k9N7L4SUMLxPrRlb7ksD/ogaEgMEYHf5QJyEdp2z4Qy2u\n"\
"C4K/+zpyKpsBeihk5NGnoqTL4LmKNZWyGvJXwq3INuhd4hCpdMgEtX1Mo2TYa8bc\n"\
"amo02VhHHX875TjxC/l3nEKXNcddLN3VvlnYGu7xvIwmCB0P9VDIB53MofqlN+lM\n"\
"zcx8XelCSSWiJKFKXCv4bsvqJOTHuejmi6yDZF8icM8zdVsCAwEAAaOCAQwwggEI\n"\
"MBIGA1UdEwEB/wQIMAYBAf8CAQAwMgYDVR0fBCswKTAnoCWgI4YhaHR0cDovL2Ny\n"\
"bC5jZXJ0dW0ucGwvZ3NjYXNoYTIuY3JsMG4GCCsGAQUFBwEBBGIwYDAoBggrBgEF\n"\
"BQcwAYYcaHR0cDovL3N1YmNhLm9jc3AtY2VydHVtLmNvbTA0BggrBgEFBQcwAoYo\n"\
"aHR0cDovL3JlcG9zaXRvcnkuY2VydHVtLnBsL2dzY2FzaGEyLmNlcjAfBgNVHSME\n"\
"GDAWgBRUmd2b/+inDqMZnVu+QlffMPyPMjAdBgNVHQ4EFgQUPZG2zBF76+RmEazS\n"\
"0gfLqaSAczEwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3DQEBCwUAA4IBAQBBGurH\n"\
"V6ciSTVqiNgltiQilglZfJcBo9MpMYDIFUnPvZKLjeMYYgbySA77cEyevma053Nm\n"\
"leNWr9burXQqbZ8P9n4iJh3n20E+A6048bsH+0mMY2Xyc1AGy4pclIUNRlPNtYpA\n"\
"H2jndvUoZP+4RwUjiGLEtX1jhORw4rZb6j+UXVWTBssj7Y+8GBp9MXl9ShIpb3Vb\n"\
"DIRnxv2KcydLRRAb4pP3mw0rHWLmAZ3UY1Aj6aeeoWh5sKZbmHOpjelwaGyiojSm\n"\
"dg0GZufYhBeYxQNXpdhWxDRIzAT890wqp77PXKTFMZaS0QaZz7k4o/K/VGJVWWkY\n"\
"1xuL4Zl8WvEcVWEK\n"\
"-----END CERTIFICATE-----"

#define SERVER_ROOT_CERT   \
"-----BEGIN CERTIFICATE-----\n"\
"MIIDuzCCAqOgAwIBAgIDBETAMA0GCSqGSIb3DQEBBQUAMH4xCzAJBgNVBAYTAlBM\n"\
"MSIwIAYDVQQKExlVbml6ZXRvIFRlY2hub2xvZ2llcyBTLkEuMScwJQYDVQQLEx5D\n"\
"ZXJ0dW0gQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkxIjAgBgNVBAMTGUNlcnR1bSBU\n"\
"cnVzdGVkIE5ldHdvcmsgQ0EwHhcNMDgxMDIyMTIwNzM3WhcNMjkxMjMxMTIwNzM3\n"\
"WjB+MQswCQYDVQQGEwJQTDEiMCAGA1UEChMZVW5pemV0byBUZWNobm9sb2dpZXMg\n"\
"Uy5BLjEnMCUGA1UECxMeQ2VydHVtIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MSIw\n"\
"IAYDVQQDExlDZXJ0dW0gVHJ1c3RlZCBOZXR3b3JrIENBMIIBIjANBgkqhkiG9w0B\n"\
"AQEFAAOCAQ8AMIIBCgKCAQEA4/t9o3K6wvDJFIf1awFO4W5AB7ptJ11/91sts1rH\n"\
"UV+rpDKmYYe2bg+G0jACl/jXaVehGDldamR5xgFZrDwxSjh80gTSSyjoIF87B6LM\n"\
"TXPb865Px1bVWqeWifrzq2jUI4ZZJ88JJ7ysbnKDHDBy3+Ci6dLhdHUZvSqeexVU\n"\
"BBvXQzmtVSjF4hq79MDkrjhJM8x2hZ85RdKknvISjFH4fOQtf/WsX+sWn7Et0brM\n"\
"kUJ3TCXJkDhv2/DM+44el1k+1WBO5gUo7Ul5E0u6SNsv+XLTOcr+H9g0cvW0QM8x\n"\
"AcPs3hEtF10fuFDRXhmnad4HMyjKUJX5p1TLVIZQRan5SQIDAQABo0IwQDAPBgNV\n"\
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBQIds3LB/8k9sXN7buQvOKEN0Z19zAOBgNV\n"\
"HQ8BAf8EBAMCAQYwDQYJKoZIhvcNAQEFBQADggEBAKaorSLOAT2mo/9i0Eidi15y\n"\
"sHhE49wcrwn9I0j6vSrEuVUEtRCjjSfeC4Jj0O7eDDd5QVsisrCaQVymcODU0HfL\n"\
"I9MA4GxWL+FpDQ3Zqr8hgVDZBqWo/5U30Kr+4rP1mS1FhIrlQgnXdAIv94nYmem8\n"\
"J9RHjboNRhx3zxSkHLmkMcScKHQDNP8zGSal6Q10tz6XxnboJ5ajZt3hrvJBW8qY\n"\
"VoNzcOSGGtIxQbovvi0TWnZvTuhOgQ4/WwMioBK+ZlgRSssDxLQqKi2WF+A5VLxI\n"\
"03YnnZotBqbJ7DnSq9ufmgsnAjUpsUCV5/nonFWIGUbWtzT1fs45mtk48VH3Tyw=\n"\
"-----END CERTIFICATE-----"

#define MQTT_TOPIC "console_device1"
// #define WEB_URL "https://www.3p3v.pl/login.html" //https://www.howsmyssl.com/a/check"

static const char *TAG = "example";

// static const char *REQUEST = "GET " WEB_URL " HTTP/1.0\r\n"
//     "Host: "WEB_SERVER"\r\n"
//     "User-Agent: esp-idf/1.0 esp32\r\n"
//     "\r\n";

void publish_callback(void** unused, struct mqtt_response_publish *published)
{

}

void mqtt_main_task(void)
{
    mbedtls_context ctx;

    const unsigned char *ca_chain[] = {(const unsigned char * )SERVER_IM2_CERT, 
                                       (const unsigned char * )SERVER_IM1_CERT, 
                                       (const unsigned char * )SERVER_ROOT_CERT,
                                       NULL};

    open_nb_socket(&ctx, WEB_SERVER, WEB_PORT, ca_chain);

    mbedtls_net_context *sockfd = &ctx.ssl_ctx;

    if (sockfd == NULL) {
        // exit_example(EXIT_FAILURE, sockfd, NULL);
        printf("err\r\n");
        goto EXIT;
    }

    struct mqtt_client client;
    uint8_t sendbuf[2048]; /* sendbuf should be large enough to hold multiple whole mqtt messages */
    uint8_t recvbuf[1024]; /* recvbuf should be large enough any whole mqtt message expected to be received */
    mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);
    mqtt_connect(&client, "publishing_client", NULL, NULL, 0, "admin", "admin", 0, 400);
    if (client.error != MQTT_OK) {
        printf("err\r\n");
        goto EXIT;
    }

    mqtt_publish(&client, MQTT_TOPIC, "test", strlen("test\r\n") + 1, MQTT_PUBLISH_QOS_2);
    if (client.error != MQTT_OK) {
        printf("err\r\n");
        goto EXIT;
    }

    EXIT:
    return;
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

    mqtt_main_task();

    // while(1);
}

// static void https_get_task()//void *pvParameters)
// {
//     // int take_break = 1;
//     // while(take_break);
    
//     char buf[512];
//     int ret, flags, len;

//     mbedtls_entropy_context entropy;
//     mbedtls_ctr_drbg_context ctr_drbg;
//     mbedtls_ssl_context ssl;
//     mbedtls_x509_crt cacert;
//     mbedtls_ssl_config conf;
//     mbedtls_net_context server_fd;

// #ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
//     psa_status_t status = psa_crypto_init();
//     if (status != PSA_SUCCESS) {
//         // ESP_LOGE(TAG, "Failed to initialize PSA crypto, returned %d", (int) status);
//         return;
//     }
// #endif

//     mbedtls_esp_enable_debug_log(&conf, 5);
    
//     mbedtls_ssl_init(&ssl);
//     mbedtls_x509_crt_init(&cacert);
//     mbedtls_ctr_drbg_init(&ctr_drbg);
//     // ESP_LOGI(TAG, "Seeding the random number generator");

//     mbedtls_ssl_config_init(&conf);

//     mbedtls_entropy_init(&entropy);
//     if((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
//                                     NULL, 0)) != 0)
//     {
//         // ESP_LOGE(TAG, "mbedtls_ctr_drbg_seed returned %d", ret);
//         abort();
//     }

//     // ESP_LOGI(TAG, "Attaching the certificate bundle...");

//     ret = esp_crt_bundle_attach(&conf);

//     if(ret < 0)
//     {
//         // ESP_LOGE(TAG, "esp_crt_bundle_attach returned -0x%x", -ret);
//         abort();
//     }

//     // ESP_LOGI(TAG, "Setting hostname for TLS session...");

//      /* Hostname set here should match CN in server certificate */
//     if((ret = mbedtls_ssl_set_hostname(&ssl, WEB_SERVER)) != 0)
//     {
//         // ESP_LOGE(TAG, "mbedtls_ssl_set_hostname returned -0x%x", -ret);
//         abort();
//     }

//     // ESP_LOGI(TAG, "Setting up the SSL/TLS structure...");

//     if((ret = mbedtls_ssl_config_defaults(&conf,
//                                           MBEDTLS_SSL_IS_CLIENT,
//                                           MBEDTLS_SSL_TRANSPORT_STREAM,
//                                           MBEDTLS_SSL_PRESET_DEFAULT)) != 0)
//     {
//         // ESP_LOGE(TAG, "mbedtls_ssl_config_defaults returned %d", ret);
//         // goto exit;
//     }

//     mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);//MBEDTLS_SSL_VERIFY_REQUIRED);
//     mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
//     mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

//     const char *protocols[] = {"h2", "http/1.1", NULL};
//     mbedtls_ssl_conf_alpn_protocols(&conf, protocols);
// #ifdef CONFIG_MBEDTLS_DEBUG
//     mbedtls_esp_enable_debug_log(&conf, CONFIG_MBEDTLS_DEBUG_LEVEL);
// #endif

// #ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
//     mbedtls_ssl_conf_min_tls_version(&conf, MBEDTLS_SSL_VERSION_TLS1_3);
//     mbedtls_ssl_conf_max_tls_version(&conf, MBEDTLS_SSL_VERSION_TLS1_3);
// #endif
//     if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0)
//     {
//         // ESP_LOGE(TAG, "mbedtls_ssl_setup returned -0x%x", -ret);
//         goto exit;
//     }

//     // while(1) {
//         mbedtls_net_init(&server_fd);

//         // ESP_LOGI(TAG, "Connecting to %s:%s...", WEB_SERVER, WEB_PORT);

//         if ((ret = mbedtls_net_connect(&server_fd, WEB_SERVER,
//                                       WEB_PORT, MBEDTLS_NET_PROTO_TCP)) != 0)
//         {
//             // ESP_LOGE(TAG, "mbedtls_net_connect returned -%x", -ret);
//             goto exit;
//         }

//         // // ESP_LOGI(TAG, "Connected.");

//         mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

//         // ESP_LOGI(TAG, "Performing the SSL/TLS handshake...");

//         while ((ret = mbedtls_ssl_handshake(&ssl)) != 0)
//         {
//             if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE)
//             {
//                 // ESP_LOGE(TAG, "mbedtls_ssl_handshake returned -0x%x", -ret);
//                 goto exit;
//             }
//         }

//         // ESP_LOGI(TAG, "Verifying peer X.509 certificate...");

//         if ((flags = mbedtls_ssl_get_verify_result(&ssl)) != 0)
//         {
//             /* In real life, we probably want to close connection if ret != 0 */
//             ESP_LOGW(TAG, "Failed to verify peer certificate!");
//             bzero(buf, sizeof(buf));
//             mbedtls_x509_crt_verify_info(buf, sizeof(buf), "  ! ", flags);
//             ESP_LOGW(TAG, "verification info: %s", buf);
//         }
//         else {
//             // ESP_LOGI(TAG, "Certificate verified.");
//         }

//         // ESP_LOGI(TAG, "Cipher suite is %s", mbedtls_ssl_get_ciphersuite(&ssl));

//         // ESP_LOGI(TAG, "Writing HTTP request...");

//         size_t written_bytes = 0;
//         do {
//             ret = mbedtls_ssl_write(&ssl,
//                                     (const unsigned char *)REQUEST + written_bytes,
//                                     strlen(REQUEST) - written_bytes);
//             if (ret >= 0) {
//                 // ESP_LOGI(TAG, "%d bytes written", ret);
//                 written_bytes += ret;
//             } else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE && ret != MBEDTLS_ERR_SSL_WANT_READ) {
//                 // ESP_LOGE(TAG, "mbedtls_ssl_write returned -0x%x", -ret);
//                 goto exit;
//             }
//         } while(written_bytes < strlen(REQUEST));

//         // ESP_LOGI(TAG, "Reading HTTP response...");

//         do
//         {
//             len = sizeof(buf) - 1;
//             bzero(buf, sizeof(buf));
//             ret = mbedtls_ssl_read(&ssl, (unsigned char *)buf, len);

// #if CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS
//             // if (ret == MBEDTLS_ERR_SSL_RECEIVED_NEW_SESSION_TICKET) {
//             //     ESP_LOGD(TAG, "got session ticket in TLS 1.3 connection, retry read");
//             //     continue;
//             // }
// #endif // CONFIG_MBEDTLS_SSL_PROTO_TLS1_3 && CONFIG_MBEDTLS_CLIENT_SSL_SESSION_TICKETS

//             if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
//                 continue;
//             }

//             if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
//                 ret = 0;
//                 break;
//             }

//             if (ret < 0) {
//                 // ESP_LOGE(TAG, "mbedtls_ssl_read returned -0x%x", -ret);
//                 break;
//             }

//             if (ret == 0) {
//                 // ESP_LOGI(TAG, "connection closed");
//                 break;
//             }

//             len = ret;
//             ESP_LOGD(TAG, "%d bytes read", len);
//             /* Print response directly to stdout as it is read */
//             for (int i = 0; i < len; i++) {
//                 putchar(buf[i]);
//             }
//         } while(1);

//         mbedtls_ssl_close_notify(&ssl);

//     exit:
//         mbedtls_ssl_session_reset(&ssl);
//         mbedtls_net_free(&server_fd);

//         if (ret != 0) {
//             mbedtls_strerror(ret, buf, 100);
//             // ESP_LOGE(TAG, "Last error was: -0x%x - %s", -ret, buf);
//         }

//         putchar('\n'); // JSON output doesn't have a newline at end

//         static int request_count;
//         // ESP_LOGI(TAG, "Completed %d requests", ++request_count);
//         printf("Minimum free heap size: %" PRIu32 " bytes\n", esp_get_minimum_free_heap_size());

//         for (int countdown = 10; countdown >= 0; countdown--) {
//             // ESP_LOGI(TAG, "%d...", countdown);
//             vTaskDelay(1000 / portTICK_PERIOD_MS);
//         }
//         // ESP_LOGI(TAG, "Starting again!");
//     // }
// }

// void app_main(void)
// {
//     struct timeval tv_now;
//     struct timezone t_z = {.tz_dsttime = 2, .tz_minuteswest = -39};
//     gettimeofday(&tv_now, NULL);
//     tv_now.tv_sec = 1694784830;
//     settimeofday(&tv_now, NULL);
//     gettimeofday(&tv_now, NULL);
//     printf("%lli", tv_now.tv_sec);

//     SIM_intf sim_obj;
//     sim = &sim_obj;
//     LL_SIM_def(sim);
//     printf("END1\r\n");
//     unsigned char arr[12000] = {};
//     sim->buf = arr;
//     sim->buf_len = 12000;
//     LL_SIM_init(sim);
//     LL_SIM_listen(sim);

//     SIM_error err;
//     SIM_cmd cmd;

//     err = SIM_run(sim, SIM_execATE0(&cmd));

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

//     printf("%i", err);
    
//     ESP_ERROR_CHECK( nvs_flash_init() );
//     // ESP_ERROR_CHECK(esp_netif_init());
//     ESP_ERROR_CHECK(esp_event_loop_create_default());

//     // /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
//     //  * Read "Establishing Wi-Fi or Ethernet Connection" section in
//     //  * examples/protocols/README.md for more information about this function.
//     //  */
//     // // ESP_ERROR_CHECK(example_connect());

//     https_get_task();

//     // while(1);
// }

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
