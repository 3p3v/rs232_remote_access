#pragma once

#include <SIM_common.h>

/* Disable echo */
#define SIM_EXECATE0_TIMEOUT 100
SIM_cmd *SIM_execATE0(SIM_cmd *cmd);

/* Check communication with module */
#define SIM_EXECAT_TIMEOUT 100
SIM_cmd *SIM_execAT(SIM_cmd *cmd);

/* AT+CREG Network Registration */
#define SIM_READCREG_TIMEOUT 1000
typedef enum SIM_CREG_stat
{
    SIM_CREG_stat_notRegMT = 0,
    SIM_CREG_stat_regHomeNet,
    SIM_CREG_stat_notReg,
    SIM_CREG_stat_regDenied,
    SIM_CREG_stat_unkonown,
    SIM_CREG_stat_regRoam
} SIM_CREG_stat;
SIM_cmd *SIM_readCREG(SIM_cmd *cmd);

/* AT+CCLK Clock */
#define SIM_READCCLK_TIMEOUT 1000
SIM_cmd *SIM_readCCLK(SIM_cmd *cmd);