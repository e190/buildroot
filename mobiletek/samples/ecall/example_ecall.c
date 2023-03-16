#include "mbtk_type.h"
#include "lynq_ecall_api.h"

#if 1
int main(int argc, char *argv[])
{
    int ret;
    ecall_handle_type handle;
    LYNQ_ECall_Indication eCall_Indication;
    const char *addr = "18349164042";
    const unsigned char *msd_data = "0220040695F3997CE65F39959F3997CE6567FCC0002958F000000530000005200020000000000000000000000000000000000000000000000000000000000000000000000000";
    LYNQ_ECall_Category ecall_cat = LYNQ_EMER_CAT_MANUAL_ECALL;
    LYNQ_ECall_Variant ecall_variant = LYNQ_ECALL_TEST;
    LYNQ_ECall_Set_Type type = LYNQ_ECALL_SET_NUMBER;

    printf("--------->[%s,%d] start\n", __FUNCTION__, __LINE__);
    ret = lynq_ecall_init(&handle);
    sleep(5);

    /**********set test number**********/
    printf("lynq_set_test_numl!!! type is %d, test_num is %s, test_num_length is %d\n",type,addr,strlen(addr)+1);

    ret = lynq_set_test_num(type, addr, strlen(addr) + 1);
    if(ret == 0)
    {
        printf("set test number success\n");
    }else{
        printf("set test number error\n");
    }

    sleep(5);
    /**********dail fast call**********/
    printf("Dial a fast call!!! Addr is %s, ecall_cat is %d, ecall_variant is %d, msd is %s\n", addr,ecall_cat,ecall_variant,msd_data);

    ret = lynq_fast_ecall(&handle,
                          (LYNQ_ECall_Category) ecall_cat,
                          (LYNQ_ECall_Variant) ecall_variant,
                          NULL, 0,
                          msd_data, strlen(msd_data));
    if(ret == 0)
    {
        printf("dial fast call success, handle is %d\n", handle);
    }else{
        printf("dial fast call error\n");
    }

    sleep(2);

    while(1)
    {
        lynq_wait_ecall_indication(&handle, &eCall_Indication);
        printf("recvd ecall indication %d\n", eCall_Indication);
        if (eCall_Indication == LYNQ_ECALL_SENDING_START)
        {
            /**********set New MSD**********/
            printf("set new msd !!! msd is %s\n", msd_data);
            ret = lynq_set_msd(&handle, msd_data, strlen(msd_data));
            if (ret == 0) {
                printf("set new msd success!!!\n");
            } else {
                printf("set new msd fail\n");
            }
            /**********set MSD --end**********/
        } else if (eCall_Indication == LYNQ_ECALL_LLACK_RECEIVED)
        {
            printf("recvd LYNQ_ECALL_LLACK_RECEIVED, msd is recved by psap\n");
            break;
        }
    }

    sleep(2);

    ret = lynq_ecall_hungup(&handle);
    if(ret == 0)
    {
        printf("hungup call success!!!\n");
    }else{
        printf("hungup call error\n");
    }
    sleep(5);

    ret = lynq_ecall_deinit(handle);
    if(ret == 0)
    {
        printf("deinit call success!!!\n");
    }else{
        printf("deinit call error\n");
    }

    return 0;
}
#else
int main(int argc, char *argv[])
{
    char operator[10];
    int opt;
    int    ret  = -1;
    ecall_handle_type handle;
    const unsigned char *msd_data = "0220040695F3997CE65F39959F3997CE6567FCC0002958F000000530000005200020000000000000000000000000000000000000000000000000000000000000000000000000";
    while(1)
    {
        printf("=========ecall main=========\n"
            "\t0 exit\n"
            "\t1 ecall init\n"
            "\t2 ecall lynq_set_test_num\n"
            "\t3 ecall lynq_fast_ecall\n"
            "\t4 ecall lynq_set_msd\n"
            "\t5 ecall lynq_ivs_push_msd\n"
            "\t6 ecall lynq_ecall_hungup\n"
            "\t7 ecall deinit\n"
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
            ret = lynq_ecall_init(&handle);
            if(ret < 0)
            {
                printf("QL_Voice_Call_Client_Init FAIL.	ret:%d\n",ret);
                return -1;
            }
            printf("QL_Voice_Call_Client_Init ret = %d, with h_voice=%d\n", ret, handle);

            break;
        case 2:
        {
            char *addr = "18349164042";
            ret = lynq_set_test_num( LYNQ_ECALL_SET_NUMBER, addr, strlen(addr) + 1);
            if(ret < 0)
            {
                printf("QL_Voice_Call_AddCommonStateHandler FAIL.		ret:%d\n",ret);
                return -1;
            }
            break;
        }
        case 3:
        {
            LYNQ_ECall_Category ecall_cat = LYNQ_EMER_CAT_MANUAL_ECALL;
            LYNQ_ECall_Variant ecall_variant = LYNQ_ECALL_TEST;
            ret = lynq_fast_ecall(&handle,
                                ecall_cat,
                                ecall_variant,
                                  NULL, 0,
                                  msd_data, strlen(msd_data));
            if (ret == 0) {
                printf("dial fast call success, handle is %d\n", handle);
            } else {
                printf("dial fast call error\n");
            }
            break;
        }
        case 4:
        {
            ret = lynq_set_msd((int *)&handle, msd_data, strlen(msd_data));
            if (ret == 0) {
                printf("set new msd success!!!\n");
            } else {
                printf("set new msd fail");
            }
            break;
        }
        case 5:
            ret = lynq_ivs_push_msd();
            if(ret == 0)
            {
                printf("push msd success!!!\n");
            }else{
                printf("push msd error\n");
            }
            break;
        case 6:
            ret = lynq_ecall_hungup(&handle);
            if(ret == 0)
            {
                printf("hungup call success!!!\n");
            }else{
                printf("hungup call error\n");
            }
            break;
        case 7:
            ret = lynq_ecall_deinit(handle);
            if(ret == 0)
            {
                printf("deinit call success!!!\n");
            }else{
                printf("deinit call error\n");
            }
            break;
        default:
            break;
        }

        sleep(1);
    }
    return 0;
}

#endif
