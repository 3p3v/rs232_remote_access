#include <auth.h>
#include <string.h>
#include <memory.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <esp_mac.h>
#include <esp_system.h>

static char *auth_load_username()
{
    unsigned char mac[6];
    ESP_ERROR_CHECK(esp_base_mac_addr_get(mac));
    char **username = auth_get_username();
    *username = realloc(*username, sizeof(char) * (MAC_LEN));

    sprintf(*username, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    return *username;
}

char **auth_get_username()
{
    static char *username = NULL;
    return &username;
}

char *auth_free_username()
{
    char **username = auth_get_username();
    free(*username);
    return *username;
}

/* PASSWORD SECTION */

static char *auth_load_password()
{
    /* Get password ptr */
    char **password = auth_get_password();
    *password = realloc(*password, sizeof(char) * (DEF_PASSWORD_LEN));
    /* Load password from nvs */
    strcpy(*password, DEF_PASSWORD);

    return *password;

    return *password;
}

char **auth_get_password()
{
    static char *password = NULL;
    return &password;
}

// char *auth_set_save_password(const char *new_password)
// {
//     /* Open nvs */
//     nvs_handle_t nvs_handle;
//     ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle));
    
//     /* Set password in nvs */
//     char **password = auth_get_password();
//     free(*password);
//     unsigned int password_len = strlen(new_password) + 1;
//     ESP_ERROR_CHECK(nvs_set_u32(nvs_handle, PASSWORD_LEN_S, password_len));
//     ESP_ERROR_CHECK(nvs_set_str(nvs_handle, PASSWORD_S, new_password));
//     ESP_ERROR_CHECK(nvs_commit(nvs_handle));
//     /* Set password in RAM */
//     *password = realloc(*password, sizeof(char) * (password_len));
//     ESP_ERROR_CHECK(nvs_get_str(nvs_handle, PASSWORD_S, *password, &password_len));

//     /* Close nvs */
//     nvs_close(&nvs_handle);

//     return *password;
// }

char *auth_free_password()
{
    char **password = auth_get_password();
    free(*password);
    return *password;
}

/* WEB SERVER SECTION */

static char *auth_load_server()
{
    /* Get web_server ptr */
    char **web_server = auth_get_server();
    *web_server = realloc(*web_server, sizeof(char) * (DEF_WEB_SERVER_LEN));
    /* Load password from nvs */
    strcpy(*web_server, DEF_WEB_SERVER);

    return *web_server;
}

char **auth_get_server()
{
    static char *server = NULL;
    return &server;
}

// char *auth_set_save_server(const char *new_server)
// {
//     /* Open nvs */
//     nvs_handle_t nvs_handle;
//     ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, nvs_handle));
    
//     /* Set web_server in nvs */
//     char **web_server = auth_get_server();
//     free(*web_server);
//     unsigned int web_server_len = strlen(new_server) + 1;
//     ESP_ERROR_CHECK(nvs_set_u32(nvs_handle, WEB_SERVER_LEN_S, web_server_len));
//     ESP_ERROR_CHECK(nvs_set_str(nvs_handle, WEB_SERVER_S, new_server));
//     ESP_ERROR_CHECK(nvs_commit(nvs_handle));
//     /* Set web_server in RAM */
//     *web_server = realloc(*web_server, sizeof(char) * (web_server_len));
//     ESP_ERROR_CHECK(nvs_get_str(nvs_handle, WEB_SERVER_S, web_server, &web_server_len));

//     /* Close nvs */
//     nvs_close(&nvs_handle);

//     return *web_server;
// }

char *auth_free_server()
{
    char **server = auth_get_server();
    free(*server);
    return *server;
}

/* PORT SECTION */

static char *auth_load_port()
{
    /* Get port ptr */
    char **port = auth_get_port();
    *port = realloc(*port, sizeof(char) * PORT_LEN);
    strcpy(*port, DEF_PORT);
    return *port;
}

char **auth_get_port()
{
    static char *port = NULL;
    return &port;
}

// char *auth_set_save_port(const char *new_port)
// {
//     /* Open nvs */
//     nvs_handle_t nvs_handle;
//     ESP_ERROR_CHECK(nvs_open(NVS_NAMESPACE, NVS_READWRITE, nvs_handle));
    
//     /* Set port in nvs */
//     char **port = auth_get_port();
//     free(*port);
//     unsigned int port_len = strlen(new_port) + 1;
//     ESP_ERROR_CHECK(nvs_set_u32(nvs_handle, PORT_LEN_S, port_len));
//     ESP_ERROR_CHECK(nvs_set_str(nvs_handle, PORT_S, new_port));
//     ESP_ERROR_CHECK(nvs_commit(nvs_handle));
//     /* Set port in RAM */
//     *port = realloc(*port, sizeof(char) * (port_len));
//     ESP_ERROR_CHECK(nvs_get_str(nvs_handle, PORT_S, port, &port_len));

//     /* Close nvs */
//     nvs_close(&nvs_handle);

//     return *port;
// }

char *auth_free_port()
{
    char **port = auth_get_port();
    free(*port);
    return *port;
}

void auth_load()
{
    /* Load MAC as username */
    auth_load_username();
    /* Load password */
    auth_load_password();
    auth_load_server();
    auth_load_port();
}
