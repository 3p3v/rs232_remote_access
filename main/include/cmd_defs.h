#pragma once

#include <stddef.h>

/* Channels */
#define INFO_CH_C 'i'
#define SET_CH_C 's'
#define DATA_CH_C 'd'

/* Set commands */
#define SET_BAUD_RATE "S_B"
#define SET_PARITY "S_P"
#define SET_CHAR_SIZE "S_C"
#define SET_STOP_BITS "S_S"

/* Get info */
#define GET_INFO "G_I"

/* Give commands */
#define GET_BAUD_RATE "G_B"
#define GET_PARITY "G_P"
#define GET_CHAR_SIZE "G_C"
#define GET_STOP_BITS "G_S"

/* Hi commands */
#define MASTER_HI "M_HI"
#define SLAVE_HI "S_HI"

/* Keep Alive */
#define MASTER_KEEP_ALIVE "M_KA"
#define SLAVE_KEEP_ALIVE "S_KA"

/* RTS/CTS */
#define CTS_SET "C_S"
#define CTS_RESET "C_R"
#define RTS_SET "E_S"
#define RTS_RESET "E_R"

/* Baud rate */
#define BAUD_RATE_MIN 9600
#define BAUD_RATE_MAX 115200

/* Parity */
#define PARITY_EVEN "E"
#define PARITY_NONE "N"
#define PARITY_ODD "O"
#define PARITY_EVEN_C 'E'
#define PARITY_NONE_C 'N'
#define PARITY_ODD_C 'O'

/* Char size */
#define CHAR_SIZE_MIN 5
#define CHAR_SIZE_MAX 8

/* STOP_BITS */
#define STOP_BITS_ONE "O"
#define STOP_BITS_ONEPOINTFIVE "OPF"
#define STOP_BITS_TWO "T"
#define STOP_BITS_ONE_C 'O'
#define STOP_BITS_ONEPOINTFIVE_C0 'O'
#define STOP_BITS_ONEPOINTFIVE_C1 'P'
#define STOP_BITS_ONEPOINTFIVE_C2 'F'
#define STOP_BITS_TWO_C 'T'

/* New session */
// #define NEW_SESSION "N_S"   // Set by master

/* RTS/CTS */
#define SET_RTS "S_R"       // Set by master
#define RESET_RTS "R_R"     // Set by master
#define SET_CTS "S_C"       // Set by device
#define RESET_CTS "R_C"     // Set by device

// /* Enable/disable emulated flow control */
// #define SET_FLOW_CTRL "S_F"

// /* FOLOW_CTRL */
// #define FLOW_CTRL_HARDWARE "H"
// #define FLOW_CTRL_SOFTWARE "S"
// #define FLOW_CTRL_NONE "N"
// #define FLOW_CTRL_HARDWARE_C 'H'
// #define FLOW_CTRL_SOFTWARE_C 'S'
// #define FLOW_CTRL_NONE_C 'N'

// typedef enum mqtt_flow_ctrl
// {
//     none = 0,
//     hardware,
//     software
// } mqtt_flow_ctrl;

/* MODE */
#define MODE_DTE "DTE"
#define MODE_DCE "DCE"

typedef enum dev_mode
{
    dte = 0,
    dce
} dev_mode;

/* PACKET NUMBER KEY */
#define PACKET_NUM_KEY "P"
#define PACKET_NUM_KEY_C 'P'
#define PACKET_NUM_MAX 1

/** ERRORS **/
/* UNKNOWN CMD */
#define UNKNOWN_CMD "U_C"
/* ILL-FORMED */
#define ILL_FORMED_CMD "IF_C"
/* INVALID ARGUMENT */
#define INVALID_ARGUMENT "I_A"
#define INVALID_OPTION "I_O"
/* INVALID PACKET NUMBER */
#define INVALID_PACKET_NUM "I_N"
/* NO PACKET NUMBER */
#define NO_PACKET_NUMBER "N_N"
/*  */
#define PACKET_OVERFLOW "P_O"
/*  */
#define DEV_DISCONNECT "D_D"

/* PACKET ACK */
#define PACKET_ACK "A"
/* MAX NOT ACKNOWLEDGED */
#define MAX_NOT_ACK 20
/* MAX SAVED */
#define ACK_AFTER 10
/* MAX SAVED */
#define MAX_SAVED 30
/* MAX MASTER PACKET LENGTH */
#define MAX_MASTER_PACKET_LEN 1460
/* MAX SLAVE PACKET LENGTH */
#define MAX_SLAVE_PACKET_LEN 1460
/* MAX SLAVE DATA LENGTH */
#define MAX_SLAVE_DATA_LEN 1300

/* MAX/MIN MESSAGE NUMBER */
#define MAX_MSG_NUM '~'
#define MIN_MSG_NUM ' '

/* MAX/MIN MQTT MESAGE ID */
#define MAX_MQTT_ID 500
#define MIN_MQTT_ID 200

/**/
#define SPACE " "
#define ENDL "\n"
#define SPACE_C ' '
#define ENDL_C '\n'

volatile char *mqtt_num_up(volatile char * num);
unsigned short *mqtt_id_up(unsigned short *);
char *get_channel_name(char *dev_name, char channel_end);
/*  */
char *cmdcmp(char *cmd, char *data, size_t len);
char *cmdcmp_arg(char *cmd, char *data, size_t len);
char *cmdchr(char *data, size_t len, char c);
size_t add_cmd(char **data, size_t current_len, char *new_cmd, char *new_arg);
size_t add_cmd_none(char **data, size_t current_len, char *new_cmd);
size_t add_cmd_uint(char **data, size_t current_len, char *new_cmd, int new_arg_);