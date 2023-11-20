#pragma once

#define MAC_LEN strlen("XX:XX:XX:XX:XX:XX") + 1

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

void auth_load();

char **auth_get_username();

char *auth_free_username();

char **auth_get_password();

char *auth_free_password();

char **auth_get_server();

char *auth_free_server();

char **auth_get_port();

char *auth_free_port();
