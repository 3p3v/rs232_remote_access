#include <stdio.h>
#include <string.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
/* FreeRTOS-specyfic libraries */
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
/* ESP32-specyfic libraries */
#include <esp_event.h>
#include <esp_log.h>
#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
/* SIM800-specyfic libraries */
#include <SIM.h>
#include <SIM_TCPIP.h>
#include <SIM_common_cmds.h>
/* MbedTLS & ESP32 specyfic libraries */
#include <mbedtls/net_sockets.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/debug.h>
#include <mbedtls_sockets.h>
#include <mbedtls/platform.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/esp_debug.h>
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/error.h>
#ifdef CONFIG_MBEDTLS_SSL_PROTO_TLS1_3
#include <psa/crypto.h>
#endif
#include <esp_crt_bundle.h>
/* MQTT-C lib */
#include <mqtt.h>

/* Definitions */
#define MAIN_SECS_TILL_REBOOT 10
#define MAIN_SIM_BUF_SIZE 12000
#define MAIN_MQTT_REC_BUF_SIZE 1024
#define MAIN_MQTT_SEND_BUF_SIZE 1024

#define MAIN_USERNAME "admin"
#define MAIN_PASSWORD "admin"

#define WEB_SERVER "www.3p3v.pl"
#define WEB_PORT "8883"
#define SERVER_IM2_CERT                                                  \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIIEzTCCA7WgAwIBAgIRANBLb+XdW9Ih58dM9kaLMUYwDQYJKoZIhvcNAQELBQAw\n" \
    "fjELMAkGA1UEBhMCUEwxIjAgBgNVBAoTGVVuaXpldG8gVGVjaG5vbG9naWVzIFMu\n" \
    "QS4xJzAlBgNVBAsTHkNlcnR1bSBDZXJ0aWZpY2F0aW9uIEF1dGhvcml0eTEiMCAG\n" \
    "A1UEAxMZQ2VydHVtIFRydXN0ZWQgTmV0d29yayBDQTAeFw0xNDA5MTExMjAwMDBa\n" \
    "Fw0yNzA2MDkxMDQ2MzlaMIGDMQswCQYDVQQGEwJQTDEiMCAGA1UEChMZVW5pemV0\n" \
    "byBUZWNobm9sb2dpZXMgUy5BLjEnMCUGA1UECxMeQ2VydHVtIENlcnRpZmljYXRp\n" \
    "b24gQXV0aG9yaXR5MScwJQYDVQQDEx5DZXJ0dW0gR2xvYmFsIFNlcnZpY2VzIENB\n" \
    "IFNIQTIwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDHhvLXeA4kyBVY\n" \
    "Dr12JusjW+/HPPWpBmWnSZC9gadbNXCD/GlHrGruRWrKn0YBasbonXi5FLRon717\n" \
    "aCCDt2+7URnJx4mLGt3X73yH3I+T+COuyTFOuLId68jYV4vfBm1N/N1/KohGr8+R\n" \
    "7eT4f4agfXVQ+gDZT3pGTywqc9IId4uHaFjW+mr5vVfu7WjL8A20jOawvlqV2K4/\n" \
    "qSrjPZsqHlzf46LHgbQEK1EjTPsFrfvAQxvGpHiEIAFsS+1d2xPIEJVIxvq+KyGb\n" \
    "W/0SJIj7/SfYo+ItvJ2a4/G2JfoPF9wTCC0N8U5I/KXxjYYXBcIE9pQ16T24T+dr\n" \
    "MfPG2mYtAgMBAAGjggE+MIIBOjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQWBBRU\n" \
    "md2b/+inDqMZnVu+QlffMPyPMjAfBgNVHSMEGDAWgBQIds3LB/8k9sXN7buQvOKE\n" \
    "N0Z19zAOBgNVHQ8BAf8EBAMCAQYwLwYDVR0fBCgwJjAkoCKgIIYeaHR0cDovL2Ny\n" \
    "bC5jZXJ0dW0ucGwvY3RuY2EuY3JsMGsGCCsGAQUFBwEBBF8wXTAoBggrBgEFBQcw\n" \
    "AYYcaHR0cDovL3N1YmNhLm9jc3AtY2VydHVtLmNvbTAxBggrBgEFBQcwAoYlaHR0\n" \
    "cDovL3JlcG9zaXRvcnkuY2VydHVtLnBsL2N0bmNhLmNlcjA5BgNVHSAEMjAwMC4G\n" \
    "BFUdIAAwJjAkBggrBgEFBQcCARYYaHR0cDovL3d3dy5jZXJ0dW0ucGwvQ1BTMA0G\n" \
    "CSqGSIb3DQEBCwUAA4IBAQDRhC+yVHfF/IFcNNKbkxX1aVsCZ0fa2t+MxyFcs7KO\n" \
    "qW1fGpOOycughOFnbM+lz4Y3gt5RaOFJTm7YVUZZ3751s5tv8nhXeXfrRIpQN7Cu\n" \
    "+Nei457HlDxEUItPlkYnDbdDes/96T19cICd1TmIPekYRXiyuPW4Wgx6vykmk91x\n" \
    "LkJ0y74TzVtUofVF446qXvea95zNpzYCVMg+AOX3ZZyy9XfST6g4um+cw/Idv31d\n" \
    "bnJdBzMOgHH3uw2YMiZQgDqvNRE+wAs+PTFEIKHmBc/t1n3Shvg9e68M+5ZRM8bE\n" \
    "WGqgLqfreTgCsCQcv9MDYw9TFUbS17RdE6NtiPfszTky\n"                     \
    "-----END CERTIFICATE-----"

#define SERVER_IM1_CERT                                                  \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIIEWDCCA0CgAwIBAgIPMR1+3eSSAgi72F73Jiu6MA0GCSqGSIb3DQEBCwUAMIGD\n" \
    "MQswCQYDVQQGEwJQTDEiMCAGA1UEChMZVW5pemV0byBUZWNobm9sb2dpZXMgUy5B\n" \
    "LjEnMCUGA1UECxMeQ2VydHVtIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MScwJQYD\n" \
    "VQQDEx5DZXJ0dW0gR2xvYmFsIFNlcnZpY2VzIENBIFNIQTIwHhcNMTYwNjA4MDgw\n" \
    "MTI5WhcNMjYwNjA2MDgwMTI5WjA9MQswCQYDVQQGEwJQTDEVMBMGA1UECgwMaG9t\n" \
    "ZS5wbCBTLkEuMRcwFQYDVQQDDA5DZXJ0eWZpa2F0IFNTTDCCASIwDQYJKoZIhvcN\n" \
    "AQEBBQADggEPADCCAQoCggEBALDozq2WteNT2CsyO5CY9rCN4neWGyj+26XUYS5o\n" \
    "oM+tzFlMrbdNk3J+aI7P8Ab29qlSuPRgpHMi++AopeaOYsiwuVJZqnN2W1PC6huf\n" \
    "b9mVMv3mcxYLCmuEiY4k9N7L4SUMLxPrRlb7ksD/ogaEgMEYHf5QJyEdp2z4Qy2u\n" \
    "C4K/+zpyKpsBeihk5NGnoqTL4LmKNZWyGvJXwq3INuhd4hCpdMgEtX1Mo2TYa8bc\n" \
    "amo02VhHHX875TjxC/l3nEKXNcddLN3VvlnYGu7xvIwmCB0P9VDIB53MofqlN+lM\n" \
    "zcx8XelCSSWiJKFKXCv4bsvqJOTHuejmi6yDZF8icM8zdVsCAwEAAaOCAQwwggEI\n" \
    "MBIGA1UdEwEB/wQIMAYBAf8CAQAwMgYDVR0fBCswKTAnoCWgI4YhaHR0cDovL2Ny\n" \
    "bC5jZXJ0dW0ucGwvZ3NjYXNoYTIuY3JsMG4GCCsGAQUFBwEBBGIwYDAoBggrBgEF\n" \
    "BQcwAYYcaHR0cDovL3N1YmNhLm9jc3AtY2VydHVtLmNvbTA0BggrBgEFBQcwAoYo\n" \
    "aHR0cDovL3JlcG9zaXRvcnkuY2VydHVtLnBsL2dzY2FzaGEyLmNlcjAfBgNVHSME\n" \
    "GDAWgBRUmd2b/+inDqMZnVu+QlffMPyPMjAdBgNVHQ4EFgQUPZG2zBF76+RmEazS\n" \
    "0gfLqaSAczEwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3DQEBCwUAA4IBAQBBGurH\n" \
    "V6ciSTVqiNgltiQilglZfJcBo9MpMYDIFUnPvZKLjeMYYgbySA77cEyevma053Nm\n" \
    "leNWr9burXQqbZ8P9n4iJh3n20E+A6048bsH+0mMY2Xyc1AGy4pclIUNRlPNtYpA\n" \
    "H2jndvUoZP+4RwUjiGLEtX1jhORw4rZb6j+UXVWTBssj7Y+8GBp9MXl9ShIpb3Vb\n" \
    "DIRnxv2KcydLRRAb4pP3mw0rHWLmAZ3UY1Aj6aeeoWh5sKZbmHOpjelwaGyiojSm\n" \
    "dg0GZufYhBeYxQNXpdhWxDRIzAT890wqp77PXKTFMZaS0QaZz7k4o/K/VGJVWWkY\n" \
    "1xuL4Zl8WvEcVWEK\n"                                                 \
    "-----END CERTIFICATE-----"

#define SERVER_ROOT_CERT                                                 \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIIDuzCCAqOgAwIBAgIDBETAMA0GCSqGSIb3DQEBBQUAMH4xCzAJBgNVBAYTAlBM\n" \
    "MSIwIAYDVQQKExlVbml6ZXRvIFRlY2hub2xvZ2llcyBTLkEuMScwJQYDVQQLEx5D\n" \
    "ZXJ0dW0gQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkxIjAgBgNVBAMTGUNlcnR1bSBU\n" \
    "cnVzdGVkIE5ldHdvcmsgQ0EwHhcNMDgxMDIyMTIwNzM3WhcNMjkxMjMxMTIwNzM3\n" \
    "WjB+MQswCQYDVQQGEwJQTDEiMCAGA1UEChMZVW5pemV0byBUZWNobm9sb2dpZXMg\n" \
    "Uy5BLjEnMCUGA1UECxMeQ2VydHVtIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MSIw\n" \
    "IAYDVQQDExlDZXJ0dW0gVHJ1c3RlZCBOZXR3b3JrIENBMIIBIjANBgkqhkiG9w0B\n" \
    "AQEFAAOCAQ8AMIIBCgKCAQEA4/t9o3K6wvDJFIf1awFO4W5AB7ptJ11/91sts1rH\n" \
    "UV+rpDKmYYe2bg+G0jACl/jXaVehGDldamR5xgFZrDwxSjh80gTSSyjoIF87B6LM\n" \
    "TXPb865Px1bVWqeWifrzq2jUI4ZZJ88JJ7ysbnKDHDBy3+Ci6dLhdHUZvSqeexVU\n" \
    "BBvXQzmtVSjF4hq79MDkrjhJM8x2hZ85RdKknvISjFH4fOQtf/WsX+sWn7Et0brM\n" \
    "kUJ3TCXJkDhv2/DM+44el1k+1WBO5gUo7Ul5E0u6SNsv+XLTOcr+H9g0cvW0QM8x\n" \
    "AcPs3hEtF10fuFDRXhmnad4HMyjKUJX5p1TLVIZQRan5SQIDAQABo0IwQDAPBgNV\n" \
    "HRMBAf8EBTADAQH/MB0GA1UdDgQWBBQIds3LB/8k9sXN7buQvOKEN0Z19zAOBgNV\n" \
    "HQ8BAf8EBAMCAQYwDQYJKoZIhvcNAQEFBQADggEBAKaorSLOAT2mo/9i0Eidi15y\n" \
    "sHhE49wcrwn9I0j6vSrEuVUEtRCjjSfeC4Jj0O7eDDd5QVsisrCaQVymcODU0HfL\n" \
    "I9MA4GxWL+FpDQ3Zqr8hgVDZBqWo/5U30Kr+4rP1mS1FhIrlQgnXdAIv94nYmem8\n" \
    "J9RHjboNRhx3zxSkHLmkMcScKHQDNP8zGSal6Q10tz6XxnboJ5ajZt3hrvJBW8qY\n" \
    "VoNzcOSGGtIxQbovvi0TWnZvTuhOgQ4/WwMioBK+ZlgRSssDxLQqKi2WF+A5VLxI\n" \
    "03YnnZotBqbJ7DnSq9ufmgsnAjUpsUCV5/nonFWIGUbWtzT1fs45mtk48VH3Tyw=\n" \
    "-----END CERTIFICATE-----"

#define MQTT_TOPIC "console_device1"

/* Global variables */
SIM_intf *sim;
struct mqtt_client client; /* TLS and MQTT specyfic structures */
QueueHandle_t client_queue;

/* Struct's definitions */
typedef struct mqtt_task_grip
{
    struct mqtt_client *client;
    QueueHandle_t *queue;
} mqtt_task_grip;

/* Function decalrations */
void *client_refresher(void *client);
void publish_callback(void **unused, struct mqtt_response_publish *published);
void mqtt_main_task(void);
void return_error(const char *tag, int err);
void mqtt_deamon(void *);
TaskHandle_t *mqtt_deamon_get_task();
TaskHandle_t mqtt_deamon_start();
TaskHandle_t mqtt_deamon_stop();

/* Main */
void app_main(void)
{
    int err = ESP_OK;

    /* Set time in case it's needed to check a validity of certyficates */
    struct timeval tv_now;
    struct timezone t_z = {.tz_dsttime = 2, .tz_minuteswest = -39};
    gettimeofday(&tv_now, NULL);
    tv_now.tv_sec = 1694784830;
    settimeofday(&tv_now, NULL);

    /* Enable the persistant ESP32 memory */
    if ((err = nvs_flash_init()))
    {
        return_error("ESP", err);
    }
    /* Enable event loop */
    // ESP_ERROR_CHECK(esp_event_loop_create_default());

    /* Initialize the SIM card module */
    SIM_intf sim_obj;
    SIM_cmd cmd;
    sim = &sim_obj;
    LL_SIM_def(sim);
    unsigned char arr[12000] = {};
    sim->buf = arr;
    sim->buf_len = 12000;
    LL_SIM_init(sim);

    /* Enable a SIM deamon */
    LL_SIM_listen(sim);

    /* Enable network connection and multiple TCP streams */
    if ((err = SIM_run(sim, SIM_execATE0(&cmd))) ||
        (err = SIM_run(sim, SIM_readCGATT(&cmd))) ||
        (err = SIM_run(sim, SIM_writeCIPMUX(&cmd, 1))) ||
        (err = SIM_run(sim, SIM_writeCSTT(&cmd, "internet", NULL, NULL))) ||
        (err = SIM_run(sim, SIM_execCIICR(&cmd))) ||
        (err = SIM_run(sim, SIM_execCIFSR(&cmd))))
    {
        return_error("SIM", err);
    }

    /* Run main */
    mqtt_main_task();
}

void mqtt_main_task(void)
{
    int err = ESP_OK;
    
    /* TLS and MQTT specyfic structures */
    mbedtls_context ctx;
    client_queue = xQueueCreate(1, sizeof(SIM_error));

    /* Load certyficates */
    const unsigned char *ca_chain[] = {(const unsigned char *)SERVER_IM2_CERT,
                                       (const unsigned char *)SERVER_IM1_CERT,
                                       (const unsigned char *)SERVER_ROOT_CERT,
                                       NULL};

    /* Open encrypted socket */
    err = open_nb_socket(&ctx, WEB_SERVER, WEB_PORT, ca_chain);
    mqtt_pal_socket_handle sockfd = &ctx.ssl_ctx;
    if (sockfd == NULL) //TODO change
    {
        // exit_example(EXIT_FAILURE, sockfd, NULL);
        // printf("err\r\n");
        // goto EXIT;
        return_error("MbedTLS", err);
    }

    /* Init MQTT client */
    uint8_t sendbuf[MAIN_MQTT_SEND_BUF_SIZE]; 
    uint8_t recvbuf[MAIN_MQTT_REC_BUF_SIZE]; 
    mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), publish_callback);
    /* Create connect message */
    const char *client_id = NULL;
    const char *username = MAIN_USERNAME;
    const char *password = MAIN_PASSWORD;
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION; /* Ensure having a clean session as client id is not specyfied */
    mqtt_connect(&client, client_id, NULL, NULL, 0, username, password, connect_flags, 60);
    if (client.error != MQTT_OK)
    {
        return_error("MQTT-C", client.error);
    }
    // xQueueSend(client_queue, &client.error, portMAX_DELAY);
mqtt_sync(&client);
    /* Start deamon */
    // if (!mqtt_deamon_start())
    // {
    //     return_error("MQTT-C", 0);
    // }

    mqtt_publish(&client, MQTT_TOPIC, "test", strlen("test") + 1, MQTT_PUBLISH_QOS_0);
    if (client.error != MQTT_OK)
    {
        return_error("MQTT-C", client.error);
    }
    // xQueueSend(client_queue, &client.error, portMAX_DELAY);
mqtt_sync(&client);
    // ESP_LOGI("MAIN", "Send MSG.");
    // mqtt_sync(&client);

// EXIT:
    return;
}

void publish_callback(void **unused, struct mqtt_response_publish *published)
{
}

void mqtt_deamon(void *)
{
    SIM_error err = SIM_noErrCode;
    
    while (xQueueReceive(client_queue, &err, 100000U / portTICK_PERIOD_MS))
    {
        // xQueueReceive((mqtt_task_grip *)grip->queue, &err, 100000U / portTICK_PERIOD_MS);
        mqtt_sync(&client);
        err = SIM_noErrCode;
    }
}

TaskHandle_t *mqtt_deamon_get_task()
{
    static TaskHandle_t handle = NULL;
    return &handle;
}

TaskHandle_t mqtt_deamon_start()
{
    TaskHandle_t *handle = mqtt_deamon_get_task();
    if (!(*handle))
    {
        xTaskCreate(mqtt_deamon, "mqtt_deamon", 4096, NULL, 5, handle);
    }

    return *handle;
}

TaskHandle_t mqtt_deamon_stop()
{
    TaskHandle_t *handle = mqtt_deamon_get_task();
    if (*handle)
    {
        vTaskDelete(*handle);
    }

    return *handle;
}

void return_error(const char *tag, int err)
{
    /* Short info */
    printf("Module %s encountered unhandlable error: %i.\r\n", tag, err);
    /* Description if known */
    switch (err)
    {
    default:
    {
        break;
    }
    }
    /* Reboot device in x */
    char i = MAIN_SECS_TILL_REBOOT;
    for (;;)
    {
        printf("Reboot in %i s.\r\n", i);
        if (i != 0)
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else
        {
            break;
        }
        i--;
    }

    printf("Rebooting...\r\n");
    abort();
}


