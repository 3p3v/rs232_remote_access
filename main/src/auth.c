#include <auth.h>
#include <string.h>
#include <memory.h>

/* USERNAME SECTION */

void auth_load()
{
    char **username = auth_get_username();
    //TODO load from nvs
    *username = realloc(*username, sizeof(char) * (strlen(MAIN_USERNAME) + 1));
    memcpy(*username, MAIN_USERNAME, strlen(MAIN_USERNAME) + 1);
    
     char **password = auth_get_password();
    //TODO load from nvs
    *password = realloc(*password, sizeof(char) * (strlen(MAIN_PASSWORD) + 1));
    memcpy(*password, MAIN_PASSWORD, strlen(MAIN_PASSWORD) + 1);

    char **server = auth_get_server();
    //TODO load from nvs
    *server = realloc(*server, sizeof(char) * (strlen(WEB_SERVER) + 1));
    memcpy(*server, WEB_SERVER, strlen(WEB_SERVER) + 1);

    char **port = auth_get_port();
    //TODO load from nvs
    *port = realloc(*port, sizeof(char) * (strlen(WEB_PORT) + 1));
    memcpy(*port, WEB_PORT, strlen(WEB_PORT) + 1);
}

char *auth_load_username()
{
    char **username = auth_get_username();
    //TODO load from nvs
    *username = realloc(*username, sizeof(char) * (strlen(MAIN_USERNAME) + 1));
    return *username;
}

char **auth_get_username()
{
    static char *username = NULL;
    return &username;
}

char *auth_set_save_username(const char *new_username)
{
    char **username = auth_get_username();
    //TODO save to nvs
    username = realloc(*username, sizeof(char) * (strlen(new_username) + 1));
    return *username;
}

char *auth_free_username()
{
    char **username = auth_get_username();
    free(*username);
    return *username;
}

/* PASSWORD SECTION */

char *auth_load_password()
{
    char **password = auth_get_password();
    //TODO load from nvs
    *password = realloc(*password, sizeof(char) * (strlen(MAIN_PASSWORD) + 1));
    return *password;
}

char **auth_get_password()
{
    static char *password = NULL;
    return &password;
}

char *auth_set_save_password(const char *new_password)
{
    char **password = auth_get_password();
    //TODO save to nvs
    *password = realloc(*password, sizeof(char) * (strlen(new_password) + 1));
    return *password;
}

char *auth_free_password()
{
    char **password = auth_get_password();
    free(*password);
    return *password;
}

/* WEB SERVER SECTION */

char *auth_load_server()
{
    char **server = auth_get_server();
    //TODO load from nvs
    *server = realloc(*server, sizeof(char) * (strlen(WEB_SERVER) + 1));
    return *server;
}

char **auth_get_server()
{
    static char *server = NULL;
    return &server;
}

char *auth_set_save_server(const char *new_server)
{
    char **server = auth_get_server();
    //TODO save to nvs
    *server = realloc(*server, sizeof(char) * (strlen(new_server) + 1));
    return *server;
}

char *auth_free_server()
{
    char **server = auth_get_server();
    free(*server);
    return *server;
}

/* PORT SECTION */

char *auth_load_port()
{
    char **port = auth_get_port();
    //TODO load from nvs
    *port = realloc(*port, sizeof(char) * (strlen(WEB_PORT) + 1));
    return *port;
}

char **auth_get_port()
{
    static char *port = NULL;
    return &port;
}

char *auth_set_save_port(const char *new_port)
{
    char **port = auth_get_port();
    //TODO save to nvs
    *port = realloc(*port, sizeof(char) * (strlen(new_port) + 1));
    return *port;
}

char *auth_free_port()
{
    char **port = auth_get_port();
    free(*port);
    return *port;
}