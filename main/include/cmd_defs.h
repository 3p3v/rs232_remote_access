#pragma once

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
#define MASTER_HI "G_B"
#define SLAVE_HI "G_P"

/* Keep Alive */
#define MASTER_KEEP_ALIVE "G_B"
#define SLAVE_KEEP_ALIVE "G_P"

/* RTS/CTS */
#define CTS_SET "C_S"
#define CTS_RESET "C_R"
#define RTS_SET "E_S"
#define RTS_RESET "E_R"

/* Min/Max */
#define BAUD_RATE_MIN 9600
#define BAUD_RATE_MAX 115200

