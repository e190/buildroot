#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "mbtk_type.h"
#include "mbtk_ftp.h"
#include "mbtk_log.h"
#include "mbtk_str.h"

static int ftp_cmd_process(mbtk_ftp_handle handle, const char *cmd)
{
    if(!strncasecmp(cmd, "download", 8)) {
        // Download file: /data
        uint32 len_count = 0;
        uint32 len;
        int download_time = 0;

        uint32 file_size = mbtk_ftp_file_size(handle, "/Luo/Luo_up.txt");
        if(file_size > 0)
        {
            printf("Will download file:/data[%d]\n", file_size);
            // Start download
            len = mbtk_ftp_download_start(handle, "/Luo/Luo_up.txt", "/tmp/ftp_data", NULL);
            if(len > 0)
            {
                len_count += len;
                download_time++;
                printf("Download[time-%d] size:[%d / %d]\n", download_time, len_count, file_size);
                while (len_count < file_size
                       && download_time <= 10 // Try 10 times.
                       && (len = mbtk_ftp_download_continue(handle)) > 0)
                {
                    len_count += len;
                    download_time++;
                    printf("Download[time-%d] size:[%d / %d]\n", download_time, len_count, file_size);
                }

                printf("Download complete - [%d / %d].\n",len_count, file_size);
            }
            else
            {
                printf("FTP download fail[%d / %d].\n",len_count, file_size);
                return -1;
            }
        }else {
            printf("File error.\n");
            return -1;
        }
    }
    else if(!strncasecmp(cmd, "upload", 6)) {
        int len = 0;
        len = mbtk_ftp_upload_start(handle, "/Luo/up_ftp3.txt", "/tmp/ftp_data", 0);
        if( len != 0)
        {
            printf("FTP update fail\n");
            return -1;
        }
        else
        {
            printf("FTP update success\n");
        }
    }
    else if(!strncasecmp(cmd, "pwd", 3)) {
        char path[50] = {0};
        mbtk_ftp_error_enum err = mbtk_ftp_pwd(handle, path);
        if(FTP_ERR_SUCCESS != err) {
            printf("mbtk_ftp_pwd() fail:%d\n", err);
            return -1;
        }

        printf("PWD : %s\n", path);
    } else if(!strncasecmp(cmd, "cd ", 3)) {
        char path[50] = {0};
        memcpy(path, cmd + 3, strlen(cmd) - 3);
        mbtk_ftp_error_enum err = mbtk_ftp_cd(handle, path);
        if(FTP_ERR_SUCCESS != err) {
            printf("mbtk_ftp_cd() fail:%d\n", err);
            return -1;
        }

        printf("cd %s\n", path);
    } else if(!strncasecmp(cmd, "ls", 2)) {
        mbtk_ftp_file_info_s list_head;
        mbtk_ftp_error_enum err = mbtk_ftp_dir_ls(handle, &list_head);
        if(FTP_ERR_SUCCESS != err) {
            printf("mbtk_ftp_dir_ls() fail:%d\n", err);
            return -1;
        }

        mbtk_ftp_file_info_s *f_ptr = list_head.next;
        while(f_ptr && !str_empty(f_ptr->name)) {
            printf("%s, %s, %d\n", f_ptr->name, f_ptr->is_file ? "F" : "D", f_ptr->size);
            f_ptr = f_ptr->next;
        }
    } else {
        printf("Unknow CMD.\n");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    mbtk_ftp_handle handle = mbtk_ftp_init("58.246.1.50", 6521, FTP_AUTH_TYPE_NON, false, false);
    if(handle < 0) {
        printf("mbtk_ftp_init() fail.\n");
        return -1;
    }

    mbtk_ftp_error_enum err = mbtk_ftp_login(handle, " FTP_TST", "FTPTST_0320");
    if(err != FTP_ERR_SUCCESS) {
        printf("mbtk_ftp_login() fail:%d\n", err);
        goto ftp_exit;
    }

    printf("Login success.\nCMD:");

    char cmd[50];
    while(1) {
        memset(cmd, 0, 50);
        gets(cmd);
        if(!strcasecmp(cmd, "q")) {
            break;
        } else {
            ftp_cmd_process(handle, cmd);
        }

        printf("CMD:");
    }

    printf("FTP test success.\n");
ftp_quit:
    mbtk_ftp_quit(handle);
ftp_exit:
    mbtk_ftp_deinit(handle);
    printf("FTP test complete.\n");
    return 0;

}

