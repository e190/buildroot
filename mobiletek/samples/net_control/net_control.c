#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define LOG_TAG "mbtk_net"
#include "mbtk_type.h"
#include "mbtk_net_control.h"
#include "mbtk_log.h"


int main(int argc, char *argv[])
{
    if(argc != 2) {
        LOGE("ARG error.");
        return -1;
    }

    LOGI("net_control start.");

    if(!strcmp(argv[1], "on")) {
        if(mbtk_net_enable(TRUE)) {
            printf("Open Net fail.\n");
        } else {
            printf("Open Net success.\n");
        }
    } else if(!strcmp(argv[1], "off")) {
        if(mbtk_net_enable(FALSE)) {
            printf("Close Net fail.\n");
        } else {
            printf("Close Net success.\n");
        }
    } else if(!strcmp(argv[1], "get")) {
        mbtk_net_state_t state = mbtk_net_state_get();
        printf("Net State : %d\n", state);
    } else {
        printf("Unknown arg : %s\n", argv[1]);
    }


    LOGI("net_control exit.");
    return 0;

}


