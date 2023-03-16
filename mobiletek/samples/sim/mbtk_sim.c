#include <stdio.h>
#include "mbtk_log.h"
#include "ql_mcm_sim.h"

static sim_client_handle_type cli_handle;
int main(int argc, char *argv[])
{
    printf("Start!\n");
    LOGI("mbtk_test start:%d", argc);
    if(argc > 1)
    {
        int i;
        for(i = 1; i < argc; i++)
        {
            LOGI("ARG-%d:%s", i, argv[i]);
        }
    }


    if(QL_MCM_SIM_Client_Init(&cli_handle)) {
        LOGE("QL_MCM_SIM_Client_Init() fail.");
        return -1;
    }

    char buff[20];
    if(QL_MCM_SIM_GetIMSI(cli_handle, NULL, buff, 20)) {
        LOGE("QL_MCM_SIM_GetIMSI() fail.");
        goto fail;
    }

    printf("IMSI : %s\n", buff);

    if(QL_MCM_SIM_GetIMEI(cli_handle, buff, 20)) {
        LOGE("QL_MCM_SIM_GetIMEI() fail.");
        goto fail;
    }

    printf("IMEI : %s\n", buff);

fail:
    if(QL_MCM_SIM_Client_Deinit(cli_handle)) {
        LOGE("QL_MCM_SIM_Client_Deinit() fail.");
        return -1;
    }

    printf("End!\n");
    return 0;
}