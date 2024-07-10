#include <cmd_defs.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

volatile char *mqtt_num_up(volatile char * num)
{
    (*num)++;

    if (*num > MAX_MSG_NUM)
    {
        *num = MIN_MSG_NUM;
    }

    return num;
}

char *mqtt_num_up_(char * num)
{
    (*num)++;

    if (*num > MAX_MSG_NUM)
    {
        *num = MIN_MSG_NUM;
    }

    return num;
}

unsigned short *mqtt_id_up(unsigned short *num)
{
    (*num)++;
    
    if (*num > MAX_MQTT_ID)
    {
        *num = MIN_MQTT_ID;
    }

    return num;
}
bool mqtt_if_bigger(char dem_id, char slave_id)
{
    return !mqtt_if_smaller_or_equal(dem_id, slave_id);
}

bool mqtt_if_smaller_or_equal(char dem_id, char slave_id)
{
    char min_id = slave_id - MAX_SAVED + 1;
    
    if (min_id >= MIN_MSG_NUM)
    {
        if (dem_id <= slave_id &&  dem_id >= min_id)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        char end = min_id + MAX_MSG_NUM;

        if (dem_id <= slave_id || dem_id >= end)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

char *get_channel_name(const char *dev_name, char channel_end)
{
    char *channel_name = (char *)malloc(sizeof(char) * (strlen(dev_name) + 2));
    memcpy(channel_name, dev_name, strlen(dev_name) + 1);
    channel_name[strlen(channel_name) + 1] = '\0';
    channel_name[strlen(channel_name)] = channel_end;
    return channel_name;
}

/* Search for first occurence of char within len */
char *cmdchr(char *data, size_t len, char c)
{
    for (size_t i = 0; i < len; i++)
    {
        if (data[i] == c)
            return data + i;
    }

    return NULL;
}

/* Search for occurence of cstring at the beggining of data within len.
 * Returns ptr to argument, but doesn't check if it really exists!
 */
char *cmdcmp(char *cmd, char *data, size_t len)
{
    if (strlen(cmd) > len)
        return NULL;

    for (unsigned int i = 0; i < strlen(cmd); i++)
    {
        if (cmd[i] != data[i])
            return NULL;
    }

    return data + strlen(cmd) + 1;
}

/* Search for command with argument (so len >= strlen(cmd) + 2 to be ok) 
 * Returns ptr to argument.
 */
char *cmdcmp_arg(char *cmd, char *data, size_t len)
{
    /* Minimal length of command */
    if (strlen(cmd) + 2 > len)
        return NULL;

    for (unsigned int i = 0; i < strlen(cmd); i++)
    {
        if (cmd[i] != data[i])
            return NULL;
    }

    return data + strlen(cmd) + 1;
}

size_t add_cmd(char **data, size_t current_len, char *new_cmd, char *new_arg)
{
    size_t new_len = current_len + strlen(new_cmd) + strlen(SPACE) + strlen(new_arg) + strlen(ENDL);
    *data = realloc(*data, new_len);

    /* Add cmd */
    memcpy(*data + current_len, new_cmd, sizeof(char) * strlen(new_cmd));
    current_len += strlen(new_cmd);

    /* Add space */
    (*data)[current_len] = SPACE_C;
    current_len += strlen(SPACE);

    /* Add arg */
    memcpy(*data + current_len, new_arg, sizeof(char) * strlen(new_arg));
    current_len += strlen(new_arg);

    /* Add endl */
    (*data)[current_len] = ENDL_C;
    current_len += strlen(ENDL);

    return current_len;
}

size_t add_cmd_none(char **data, size_t current_len, char *new_cmd)
{
    size_t new_len = current_len + strlen(new_cmd) + strlen(ENDL);
    *data = realloc(*data, new_len);

    /* Add cmd */
    memcpy(*data + current_len, new_cmd, sizeof(char) * strlen(new_cmd));
    current_len += strlen(new_cmd);

    /* Add endl */
    (*data)[current_len] = ENDL_C;
    current_len += strlen(ENDL);

    return current_len;
}

size_t add_cmd_uint(char **data, size_t current_len, char *new_cmd, int new_arg_)
{
    char new_arg[10] = {0};
    sprintf(new_arg, "%u", new_arg_);

    size_t new_len = current_len + strlen(new_cmd) + strlen(SPACE) + strlen(new_arg) + strlen(ENDL);
    *data = realloc(*data, new_len);

    /* Add cmd */
    memcpy(*data + current_len, new_cmd, sizeof(char) * strlen(new_cmd));
    current_len += strlen(new_cmd);

    /* Add space */
    (*data)[current_len] = SPACE_C;
    current_len += strlen(SPACE);

    /* Add arg */
    memcpy(*data + current_len, new_arg, sizeof(char) * strlen(new_arg));
    current_len += strlen(new_arg);

    /* Add endl */
    (*data)[current_len] = ENDL_C;
    current_len += strlen(ENDL);

    return current_len;
}