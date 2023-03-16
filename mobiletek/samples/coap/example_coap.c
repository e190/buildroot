#include <string.h>
#include "mbtk_type.h"
#include "ql_vcall.h"
#include "mbtk_coap.h"
#include "mbtk_coap_api.h"

int coap_get_method(void);

int coap_post_method(void);


int main(int argc, char *argv[])
{
    char operator[10];
    int opt;
    int    ret     = -1;

	printf("coap_demo star\r\n");
	coap_get_method();

	sleep(20);
	coap_post_method();

    return 0;
}


int coap_get_method(void)
{
	int ret = -1;
	ret = mbtk_coap_ecoapnew_exec_cmd("134.102.218.18",5683,0,0);    //server ip, port
	mbtk_coap_ecoapmsgid_exec_cmd(1);								//set message id
	mbtk_coap_ecoaptype_exec_cmd(0);								//set message type
	mbtk_coap_ecoapcode_exec_cmd(1);								//set Get request method
	mbtk_coap_ecoapopt_exec_cmd("b474657374",10);					//set option

	mbtk_coap_ecoapsend_exec_cmd(1, 0, NULL);						//request Get
	return 0;
}


int coap_post_method(void)
{
	int ret = -1;
	ret = mbtk_coap_ecoapnew_exec_cmd("134.102.218.18",5683,0,0);     //server ip, port
	mbtk_coap_ecoapmsgid_exec_cmd(3);            					//set message id
	mbtk_coap_ecoaptype_exec_cmd(0);             					//set message type
	mbtk_coap_ecoapcode_exec_cmd(2);			 					//set Post request method
	mbtk_coap_ecoapopt_exec_cmd("b474657374",10);					//set option

	mbtk_coap_ecoapsend_exec_cmd(3, 7, "1111111");  				//request post
	return 0;
}
