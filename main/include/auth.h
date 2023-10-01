#pragma once

#define MAIN_USERNAME "admin"
#define MAIN_PASSWORD "admin"
#define WEB_SERVER "www.3p3v.pl"
#define WEB_PORT "8883"

void auth_load();


char *auth_load_username();


char **auth_get_username();


char *auth_set_save_username(const char *new_username);


char *auth_free_username();

/* PASSWORD SECTION */

char *auth_load_password();


char **auth_get_password();


char *auth_set_save_password(const char *new_password);


char *auth_free_password();


/* WEB SERVER SECTION */

char *auth_load_server();


char **auth_get_server();


char *auth_set_save_server(const char *new_server);

char *auth_free_server();


/* PORT SECTION */

char *auth_load_port();


char **auth_get_port();


char *auth_set_save_port(const char *new_port);


char *auth_free_port();
