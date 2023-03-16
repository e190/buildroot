#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define LOG_TAG "mbtk_http"
#include "mbtk_type.h"
#include "mbtk_http.h"
#include "mbtk_log.h"


static void http_data_cb_func(
    int session_id, mbtk_http_data_type_enum type,
    void *data,int data_len)
{
    if(type == MBTK_HTTP_DATA_HEADER) {
        printf("Header(%d):%s\n",data_len,(char*)data);
    } else if(type == MBTK_HTTP_DATA_CONTENT) {
        printf("Data(%d):%s\n",data_len,(char*)data);
    } else {
        LOGI(">>>>>Complete<<<<<\n");
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2) {
        LOGE("ARG error.");
        return -1;
    }

    LOGI("MBTK HTTP Version 1.1");

    int http_handle = mbtk_http_handle_get(TRUE, http_data_cb_func);
    if(http_handle < 0)
    {
        LOGE("mbtk_http_handle_get() fail.");
        return -1;
    }

    int http_session = mbtk_http_session_create(http_handle,HTTP_OPTION_GET,HTTP_VERSION_1_1);
    if(http_handle < 0)
    {
        LOGE("mbtk_http_session_create() fail.");
        return -1;
    }

    if(mbtk_http_session_url_set(http_handle, http_session, argv[1])) {
        LOGE("mbtk_http_session_url_set() fail.\n");
        return -1;
    }

    const mbtk_http_session_t* session = mbtk_http_session_get(http_handle, http_session);
    LOGI("HTTP:%d,%s,%d,%s\n",session->option,session->host,session->port,session->uri);


    if(mbtk_http_session_start(http_handle, http_session)) {
        LOGE("mbtk_http_session_start() fail.\n");
        return -1;
    }


    if(mbtk_http_handle_free(http_handle))
    {
        LOGE("mbtk_http_handle_free() fail.");
        return -1;
    }

    LOGI("MBTK_HTTP exit.");
    return 0;

}
