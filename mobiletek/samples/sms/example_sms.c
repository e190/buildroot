/**
 *   \file dtmf_test.c
 *   \brief A Documented file.
 *
 *  Detailed description
 *   \Author:  jinLuo
 *   \Version: 1.0.0
 *   \Date: 2022-12-1
 */

/******************************************************************************\
 *   Include files
\******************************************************************************/
#include <pthread.h>
#include <time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "lynq_sms_api.h"


int main(int argc, char *argv[])
{
    char operator[20];
    int opt;
    int ret, uToken; 
	char serNum[50] = {0};

    while(1)
    {
        printf("=========audio main=========\n"
            "\t0 exit\n"
            "\t1 sms init\n"
            "\t2 send sms\n"
            "\t3 wait receive new sms\n"
            "\t4 delete sms(int index);\n"
            "\t5 list sms\n"
            "\t6 query sms storage status\n"
            "\t7 query service number\n"
            "\t8 set service number\n"
            "\t9 deinit sms\n"
            "operator: >> ");
        fgets(operator, sizeof(operator), stdin);
        fflush(stdin);
        opt = atoi(operator);
        switch (opt)
        {
        case 0:
            printf("main exit\n");
            return 0;
        case 1:
            lynq_init_sms(uToken);
            break;
        case 2:
			printf("please input volume (0~1): \n");
            fgets(operator, sizeof(operator), stdin);
            fflush(stdin);
            opt = atoi(operator);
			if(opt)
			{
				lynq_send_sms("15775590631", 1, "nihaoma,wohenhao"); //text mode
			}
			else{
				lynq_send_sms("29", 0, "0891683108200805F011000D91685177550996F70008A80E4F60597D5417FF1F62115F88597D");  //pud mode
			}
            break;
        case 3:
            lynq_wait_receive_new_sms(&uToken);
            break;
		case 4:
            printf("please input volume (0~100): \n");
            fgets(operator, sizeof(operator), stdin);
            fflush(stdin);
            opt = atoi(operator);
			lynq_delete_sms(opt);
            break;
		case 5:
			printf("please input index (0~50): \n");
            fgets(operator, sizeof(operator), stdin);
            fflush(stdin);
            opt = atoi(operator);
            lynq_list_sms(1, opt, "ALL" ); 
				//opt : 0 ; Query all stored SMS messages
				//opt : > 0 ; Gets the content of the SMS message starting from the index number 
			break;
		case 6:
            lynq_query_sms_storage_status();
			break;
        case 7:
            ret = lynq_get_smsc_address(serNum);
            if(!ret)
                printf("get_smsc:%s\n", serNum);
            break;
        case 8:
            printf("please input service num: \n");
            fgets(operator, sizeof(operator), stdin);
            fflush(stdin);
            ret= lynq_set_smsc_address(operator);
            break;
        case 9:
            lynq_deinit_sms();
            break;
        default:
            break;
        }
    }

    return 0;
}
