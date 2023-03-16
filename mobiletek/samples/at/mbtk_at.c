#include <termios.h>
#include <pthread.h>
#include <sys/un.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

#define AT_TYPE_SOCKET
#define MBTK_LOG

#ifdef MBTK_LOG
#include "mbtk_log.h"
#else
#define LOGE printf
#define LOGD printf
#define LOGV printf
#define LOGI printf
#endif

static int epoll_fd = -1;
static struct epoll_event epoll_events[20];
static int at_fd = -1;

#ifdef AT_TYPE_SOCKET
#define TEMP_FAILURE_RETRY(exp) ({         \
    typeof (exp) _rc;                      \
    do {                                   \
        _rc = (exp);                       \
    } while (_rc == -1 && errno == EINTR); \
    _rc; })
#endif

static void at_epoll_change(int is_add,int fd)
{
    struct epoll_event ev;
    memset(&ev,0x0,sizeof(struct epoll_event));
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    if(is_add)
    {
        epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&ev);
    }
    else
    {
        epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,&ev);
    }
}

static int at_set_fd_noblock(int fd)
{
    // Set O_NONBLOCK
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        LOGE("Get flags error:%s\n", strerror(errno));
        return -1;
    }
    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) < 0) {
        LOGE("Set flags error:%s\n", strerror(errno));
        return -1;
    }

    return 0;
}

#ifdef AT_TYPE_SOCKET
int openSocket(const char* sockname)
{
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock < 0) {
		LOGE("Error create socket: %s\n", strerror(errno));
		return -1;
	}
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strlcpy(addr.sun_path, sockname, sizeof(addr.sun_path));
	while (TEMP_FAILURE_RETRY(connect(sock,(const struct sockaddr*)&addr, sizeof(addr))) != 0) {
		LOGE("Error connect to socket %s: %s, try again", sockname, strerror(errno));
		sleep(1);
	}

#if 0
	int sk_flags = fcntl(sock, F_GETFL, 0);
	fcntl(sock, F_SETFL, sk_flags | O_NONBLOCK);
#endif

	return sock;
}

#else

static int at_open(char *dev)
{
    // Init USB PC port.
    struct termios ser_settings;
    int fd = -1;

usb_pc_init:
    fd = open(dev,O_RDWR);
    if(fd <= 0)
    {
        if(errno == ENODEV)
        {
            LOGD("Wait dev[%s] ready...",dev);
            usleep(500000);
            goto usb_pc_init;
        }

        LOGE("Cannot open USB PC port[%s] - [errno = %d]",dev,errno);
        return -1;
    }

    tcgetattr(fd, &ser_settings);
    cfmakeraw(&ser_settings);
    ser_settings.c_lflag |= (ECHO | ECHONL);
    ser_settings.c_lflag &= ~ECHOCTL;
    tcsetattr(fd, TCSANOW, &ser_settings);

#if 0
    if(at_set_fd_noblock(at_fd))
    {
        LOGE("at_set_fd_noblock() fail.");
        return -1;
    }

    at_epoll_change(1, at_fd);
#endif

    return fd;
}
#endif

static void signal_process(int signal_num) {
    if(at_fd > 0) {
        close(at_fd);
    }
#ifdef MBTK_LOG
    LOGD("Exit by sig - %d\n", signal_num);
#endif
    exit(0);
}

static void* read_thread_run( void *arg)
{
    //UNUSED(arg);

    char at_rsp[1024];
    char *ptr;
    int index;
    int len;
    while(at_fd > 0) {
        memset(at_rsp, 0x0, 1024);
        index = 0;
        len = 0;
        while(1) {
            if((len = read(at_fd, at_rsp + index, 1024)) > 0) {
                ptr = at_rsp;
                while(*ptr == '\r' || *ptr == '\n')
                {
                    ptr++;
                }
                if(strlen(ptr) > 0 && ptr[strlen(ptr) - 1] == '\n') {
                    printf("<%s\n", ptr);
#ifdef MBTK_LOG
                    LOGV("RSP:%s", ptr);
#endif

                    break;
                } else {
                    index += len;
                }
            } else {
                LOGE("Read error:%d",errno);
                return NULL;
            }
        }
    }

    LOGD("read_thread_run() exit.\n");
    return NULL;
}

static int read_thread_start()
{
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&tid, &attr, read_thread_run, &attr);
    if (ret < 0)
    {
        LOGE("pthread_create\n");
        return -1;
    }

    return 0;
}


int main(int argc, char *argv[])
{
    //UNUSED(argc);
    //UNUSED(argv);

#ifndef AT_TYPE_SOCKET
    if(argc != 2)
        return 0;
#endif

#ifdef MBTK_LOG
    mbtk_log_init(NULL, "MBTK_AT");
    LOGI("mbtk_at start.");
#endif

    signal(SIGKILL, signal_process);
    signal(SIGINT, signal_process);
    signal(SIGQUIT, signal_process);
    signal(SIGTERM, signal_process);
#ifdef AT_TYPE_SOCKET
    at_fd = openSocket("/tmp/atcmdmbtk");
#else
	at_fd = at_open(argv[1]);
#endif
    if(at_fd > 0) {
        setbuf(stdout, NULL);
        read_thread_start();
        char at_cmd[100];
        //printf(">>");
        while(1)
        {
            memset(at_cmd, 0x0, 100);
            if(fgets(at_cmd, 100, stdin))
            {
                char *ptr = at_cmd + strlen(at_cmd) - 1;
                while(ptr >= at_cmd && (*ptr == '\r' || *ptr == '\n'))
                {
                    *ptr-- = '\0';
                }
                // printf("AT:[%ld]%s\n", strlen(at_cmd), at_cmd);
                if(!strncasecmp(at_cmd, "at", 2))
                {
                    *(++ptr) = '\r';
                    *(++ptr) = '\n';
                    if(write(at_fd, at_cmd, strlen(at_cmd)) <= 0) {
                        LOGE("Write error:%d",errno);
                        break;
                    }
                    printf(">%s",at_cmd);
#ifdef MBTK_LOG
                    LOGV("AT:%s",at_cmd);
#endif
                } else if(!strcasecmp(at_cmd, "q")) {
                    break;
                } else {
                    printf("\n");
                }
            }
        }

        close(at_fd);

#ifdef MBTK_LOG
        LOGD("EXIT");
#endif
    }

    return 0;
}
