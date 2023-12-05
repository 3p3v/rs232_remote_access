#include <auth.h>
#include <string.h>
#include <memory.h>
#include <esp_err.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <esp_mac.h>
#include <esp_system.h>
#include <esp_spiffs.h>
#include <stdio.h>
#include <cmd_defs.h>

static char *auth_load_username()
{
    unsigned char mac[6];
    ESP_ERROR_CHECK(esp_base_mac_addr_get(mac));
    char *username = malloc(sizeof(char) * (MAC_LEN));

    sprintf(username, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return username;
}

static void replace_char(char * str, char ch1, char ch2)
{
    for (unsigned char i = 0; i < strlen(str); i++)
    {
        if (str[i] == ch1) 
            str[i] = ch2;
    }
}

int auth_load(auth_pack *pack)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *f;

    /* Load checks */
    bool password = false;
    bool server = false;
    bool port = false;
    bool chain_size = false;

    /* Nullout pack */
    pack->password = NULL;
    pack->server = NULL;
    pack->port = NULL;

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

        return err;
    }

    ESP_LOGI(TAG, "MOUNTING OK");

    /* Check if valid */
    err = esp_spiffs_check(conf.partition_label);

    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "SPIFFS_check() failed (%s)", esp_err_to_name(err));

        return err;
    }

    ESP_LOGI(TAG, "PARTITION CHECK SUCCESSFUL");

    /* Partition info */
    size_t total = 0, used = 0;
    err = esp_spiffs_info(conf.partition_label, &total, &used);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s). Formatting...", esp_err_to_name(err));
        esp_spiffs_format(conf.partition_label);

        return err;
    }

    ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);

    /* Open conf file */
    ESP_LOGI(TAG, "OPENING CONF FILE");
    f = fopen(AUTH_CONF_PATH, AUTH_RW);

    if (f == NULL)
    {
        ESP_LOGE(TAG, "FAILED TO OPEN CONFIG FILE %s", AUTH_CONF_PATH);

        return -1;
    }

    err = ESP_OK;

    /* Load username */
    pack->username = auth_load_username();
    ESP_LOGI(TAG, "USERNAME: %s", pack->username);

    /* Load file line by line */
    char *arg_ptr;
    while ((read = __getline(&line, &len, f)) != -1)
    {
        /* Check commands */
        if ((arg_ptr = cmdcmp_arg(AUTH_PASSWORD, line, len)) != NULL)
        {
            pack->password = malloc(sizeof(char) * (len - (arg_ptr - line)));
            memcpy(pack->password, arg_ptr, (len - (arg_ptr - line)));
            pack->password[(len - (arg_ptr - line))] = '\0';

            replace_char(pack->password, '\r', '\0');

            ESP_LOGE(TAG, "PASSWOED LOADED");
            password = true;
        }
        else if ((arg_ptr = cmdcmp_arg(AUTH_SERVER, line, len)) != NULL)
        {
            pack->server = malloc(sizeof(char) * (len - (arg_ptr - line)));
            memcpy(pack->server, arg_ptr, (len - (arg_ptr - line)));
            pack->server[(len - (arg_ptr - line))] = '\0';

            replace_char(pack->server, '\r', '\0');

            ESP_LOGE(TAG, "SERVER: %s", pack->server);
            server = true;
        }
        else if ((arg_ptr = cmdcmp_arg(AUTH_PORT, line, len)) != NULL)
        {
            pack->port = malloc(sizeof(char) * (len - (arg_ptr - line)));
            memcpy(pack->port, arg_ptr, (len - (arg_ptr - line)));
            pack->port[(len - (arg_ptr - line))] = '\0';

            replace_char(pack->port, '\r', '\0');

            ESP_LOGE(TAG, "PORT: %s", pack->port);
            port = true;
        }
        else if ((arg_ptr = cmdcmp_arg(AUTH_CHAIN_SIZE, line, len)) != NULL)
        {
            /* Argument in cstring to uint */
            char *arg_str = (char *)malloc(sizeof(char) * (len - (arg_ptr - line)));
            memcpy(arg_str, arg_ptr, (len - (arg_ptr - line)));
            arg_str[(len - (arg_ptr - line))] = '\0';
            pack->chain_size = atoi(arg_str);
            free(arg_str);

            ESP_LOGE(TAG, "CHAIN SIZE: %u", pack->chain_size);
            chain_size = true;
        }
        else
        {
            ESP_LOGE(TAG, "UNKNOWN OPTION");
            err = -1;

            break;
        }
    }

    /* Deallocate line */
    if (line)
        free(line);

    /* Close file */
    fclose(f);

    /* Error occured or something was not loaded? */
    if (err != ESP_OK || (password & server & port & chain_size) == false)
    {
        ESP_LOGE(TAG, "ERROR, DEALLOCATING STRUCTURES");

        free(pack->username);
        if (pack->password != NULL)
            free(pack->password);
        else if (pack->server != NULL)
            free(pack->server);
        else if (pack->port != NULL)
            free(pack->port);
    }

    /* Close partition */
    esp_vfs_spiffs_unregister(conf.partition_label);

    return err;
}

void auth_free(auth_pack *pack)
{
    free(pack->username);
    free(pack->password);
    free(pack->server);
    free(pack->port);
}
