#pragma once

#include <auth_defs.h>

/* Files realted */
#define TAG "AUTH"
#define AUTH_TASK_NAME TAG

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

/* NVS */
#define NVS_NAMESPACE "settings"

/* Password */
#define DEF_PASSWORD "admin"
#define DEF_PASSWORD_LEN strlen(DEF_PASSWORD) + 1

/* Web server */
#define DEF_WEB_SERVER "www.3p3v.pl"
#define DEF_WEB_SERVER_LEN strlen(DEF_WEB_SERVER) + 1

/* Web port */
#define DEF_PORT "8883"
#define PORT_LEN strlen(DEF_PORT) + 1

// TODO to load from config
#define DEF_APN "internet"
#define DEF_USERNAME NULL
#define DEF_PASSWORD NULL

typedef struct auth_pack
{
    /* Mobile network */
    char *apn;
    char *apn_username;
    char *apn_password;

    /* Broker */
    char *username;
    char *password;
    char *server;
    char *port;
    unsigned char chain_size;
} auth_pack;

int auth_load(auth_pack *pack);

void auth_free(auth_pack *pack);
