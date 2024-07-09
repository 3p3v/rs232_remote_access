#pragma once

#define CERT_FILE_NAME "cert_"
#define CERT_FILE_EXT ".crt"
#define CERT_SL "/"
#define CERT_FILE_NAME_MALLLOC_LEN strlen(AUTH_PATH) + strlen(CERT_SL) + strlen(CERT_FILE_NAME) + strlen(CERT_FILE_EXT) + 1

unsigned char *cert_load_chain(unsigned char number);