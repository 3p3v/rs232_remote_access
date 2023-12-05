#pragma once

#define MAC_LEN strlen("XX:XX:XX:XX:XX:XX") + 1

/* Files realted */
#define TAG "AUTH"

/* Conf partition */
#define AUTH_LABEL "conf"
#define AUTH_PATH "/conf"
#define AUTH_MAX_FILES 10
#define AUTH_CONF_FILE "dev.conf"
#define AUTH_CONF_PATH "/conf/dev.conf"
#define AUTH_RW "r"

/* Commands */
#define AUTH_USERNAME "username"
#define AUTH_PASSWORD "password"
#define AUTH_SERVER "server"
#define AUTH_PORT "port"
#define AUTH_CHAIN_SIZE "chain_size"

// /* NVS */
#define NVS_NAMESPACE "settings"
// /* Password */
// #define PASSWORD_LEN_S "pass_len"
// #define PASSWORD_S "pass"
#define DEF_PASSWORD "admin"
#define DEF_PASSWORD_LEN strlen(DEF_PASSWORD) + 1
// /* Web server */
// #define WEB_SERVER_LEN_S "web_len"
// #define WEB_SERVER_S "web"
#define DEF_WEB_SERVER "www.3p3v.pl"
#define DEF_WEB_SERVER_LEN strlen(DEF_WEB_SERVER) + 1
// /* Web port */
// #define PORT_S "port"
#define DEF_PORT "8883"
#define PORT_LEN 4 + 1
// /* Certs */
// #define CHAIN_S "chain"
// #define CHAIN_LEN_S "chain_len"

typedef struct auth_pack
{
    char *username;
    char *password;
    char *server;
    char *port;
    unsigned char chain_size;
} auth_pack;

int auth_load(auth_pack *pack);

void auth_free(auth_pack *pack);
