#include "SIM.h"
#include <LL_SIM.h>

SIM_error SIM_reset(SIM_intf *sim)
{
    SIM_error err = SIM_ok;

    if ((err = LL_SIM_setRST(sim, true)) != SIM_ok)
        return err;

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    err = LL_SIM_setRST(sim, false);
    return err;
}