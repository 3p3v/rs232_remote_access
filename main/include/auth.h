#pragma once

#define MAIN_USERNAME "admin"
#define MAIN_PASSWORD "admin"
#define WEB_SERVER "www.3p3v.pl"
#define WEB_PORT "8883"

const char *auth_get_username();
const char *auth_set_username(const char *new_username);
const char *auth_get_password();
const char *auth_set_password(const char *new_password);