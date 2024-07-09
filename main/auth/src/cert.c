#include <cert.h>
#include <auth.h>
#include <esp_log.h>
#include <esp_err.h>
#include <esp_spiffs.h>
#include <string.h>
#include <memory.h>

/* Malloc cert */
unsigned char *cert_load_chain(unsigned char number)
{
    FILE* f;

    /* Start initialization */
    ESP_LOGI(TAG, "INITIALIZING SPIFFS PARTITION");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = AUTH_PATH,
        .partition_label = AUTH_LABEL,
        .max_files = AUTH_MAX_FILES,
        .format_if_mount_failed = true};

    /* Mount */
    esp_err_t err = esp_vfs_spiffs_register(&conf);

    if (err != ESP_OK)
    {
        if (err == ESP_FAIL)
        {
            ESP_LOGE(TAG, "FAILED TO MOUNT OR FORMAT FILE SYSTEM");
        }
        else if (err == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "PARTITION DOES NOT EXIST");
        }
        else
        {
            ESP_LOGE(TAG, "FAILED TO INITIALIZE SPIFFS (%s)", esp_err_to_name(err));
        }

        return NULL;
    }

    ESP_LOGI(TAG, "MOUNTING OK");

    /* Check if valid */
    err = esp_spiffs_check(conf.partition_label);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(err));

        return NULL;
    }

    ESP_LOGI(TAG, "PARTITION CHECK SUCCESSFUL");

    /* Partition info */
    size_t total = 0, used = 0;
    err = esp_spiffs_info(conf.partition_label, &total, &used);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(err));
        esp_spiffs_format(conf.partition_label);

        return NULL;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);

    char *file_name;
    char num[10];
    int len;
    
    /* Load cert */
    if ((len = sprintf(num, "%u", number)) <= 0)
    {
        return NULL;
    }

    /* Set cert file name */
    file_name = calloc(CERT_FILE_NAME_MALLLOC_LEN + len, sizeof(char));
    strcat(file_name, AUTH_PATH);
    strcat(file_name, CERT_SL);
    strcat(file_name, CERT_FILE_NAME);
    strcat(file_name, num);
    strcat(file_name, CERT_FILE_EXT);
    
    /* Open cert file */
    ESP_LOGI(TAG, "OPENING %s FILE", file_name);
    f = fopen(file_name, AUTH_RW);

    if (f == NULL) {
        ESP_LOGE(TAG, "FAILED TO OPEN %s FILE", file_name);
        
        return NULL;
    }
    
    /* Allocate cert */
    fseek(f, 0L, SEEK_END);
    size_t fsize = ftell(f);
    rewind(f);
    ESP_LOGE(TAG, "CERT SIZE: %u", fsize);
    char *content = malloc(fsize * sizeof(char));
    size_t loaded;
    if ((loaded = fread(content, sizeof(char), fsize, f)) != fsize)
    {
        /* Length doesn't match */
        ESP_LOGI(TAG, "LOADED %u BYTES INSTEAD OF %u, ERROR", loaded, fsize);
        free(content);
        fclose(f);
        esp_vfs_spiffs_unregister(conf.partition_label);
        return NULL;
    }

    ESP_LOGI(TAG, "SUCCESSFULLY LOADED CERT %u", number);

    /* Close file */
    fclose(f);

    /* Close partition */
    esp_vfs_spiffs_unregister(conf.partition_label);
    
    return (unsigned char *)content;
}