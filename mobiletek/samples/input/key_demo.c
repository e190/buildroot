#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <poll.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <sys/timerfd.h>
#include <time.h>

#include "mbtk_type.h"
#include "mbtk_log.h"


#define MAX_DEVICES 16
#define EVIOCSSUSPENDBLOCK _IOW('E', 0x91, int)
#define POWER_KEY_LONG_PRESS_TIMEOUT 8000  // 8s
#define POWER_KEY_PRESS_TIMEOUT 2000  // 2s

static struct pollfd ev_fds[MAX_DEVICES];
static unsigned ev_count = 0;


static int ev_init(void)
{
    DIR *dir;
    struct dirent *de;
    int fd;

    fd = open("/dev/rtc0", O_RDONLY);
    if (fd < 0)
    {
        LOGW("open rtc0 error\n");
    }
    else
    {
        ev_fds[ev_count].fd = fd;
        ev_fds[ev_count].events = POLLIN;
        ev_count++;
        LOGI("Monitor /dev/rtc0");
    }

    dir = opendir("/dev/input");
    if (dir != NULL)
    {
        LOGI("dir = /dev/input");
        while ((de = readdir(dir)) != NULL)
        {
            if (strncmp(de->d_name, "event", 5))
                continue;
            fd = openat(dirfd(dir), de->d_name, O_RDONLY);
            if (fd < 0)
                continue;

            ev_fds[ev_count].fd = fd;
            ev_fds[ev_count].events = POLLIN;
            ioctl(fd, EVIOCSSUSPENDBLOCK, 1);
            ev_count++;
            if (ev_count == MAX_DEVICES)
                break;

            LOGI("Monitor /dev/input/%s", de->d_name);
        }

        closedir(dir);
    }
    else
    {
        LOGE("opendir() fail.[%d]",errno);
        return -1;
    }
    return 0;
}

static void ev_exit(void)
{
    while (ev_count > 0)
    {
        close(ev_fds[--ev_count].fd);
    }
}

/* wait: 0 dont wait; -1 wait forever; >0 wait ms */
static int ev_get(struct input_event *ev, int wait_ms)
{
    int r;
    unsigned n;
    unsigned long alarm_data;

//    if(wait_ms < 0)
//    {
//        LOGE("poll event return\n");
//        return -1;
//    }

    LOGI("Waitting data...");
    r = poll(ev_fds, ev_count, wait_ms);
    LOGI("Get Data:result = %d",r);

    if (r > 0)
    {
        for (n = 0; n < ev_count; n++)
        {
            if (ev_fds[n].revents & POLLIN)
            {
                if (n == 0)
                {
                    r = read(ev_fds[n].fd, &alarm_data, sizeof(alarm_data));
                    LOGD("get form 0 is %ld", alarm_data);
                    ev->type = EV_KEY;
                    ev->code = KEY_BRL_DOT8;
                    ev->value = 1;
                    return 0;
                }
                else
                {
                    r = read(ev_fds[n].fd, ev, sizeof(*ev));
                    if (r == sizeof(*ev))
                        return 0;
                }
            }
        }
    }
    return -1;
}


static void power_process(bool down, struct timeval *time)
{
    printf("POWER_KEY - %s,Time : %ld %ld \n", down ? "DOWN" : "UP", time->tv_sec, time->tv_usec);
#if 0
	static struct timeval down_time;
    static bool key_down;
    if(down)  // Down
    {
        key_down = true;
        power_long_press_timeout = false;
        down_time.tv_sec = time->tv_sec;
        down_time.tv_usec = time->tv_usec;
        signal(SIGALRM, power_key_timer_alrm);
        struct itimerval val;
        // Only time
        val.it_interval.tv_sec  = 0;
        val.it_interval.tv_usec = 0;
        // Time
        if(POWER_KEY_LONG_PRESS_TIMEOUT >= 1000)
        {
            val.it_value.tv_sec  = POWER_KEY_LONG_PRESS_TIMEOUT/1000;
            val.it_value.tv_usec = POWER_KEY_LONG_PRESS_TIMEOUT%1000;
        }
        else
        {
            val.it_value.tv_sec  = 0;
            val.it_value.tv_usec = POWER_KEY_LONG_PRESS_TIMEOUT;
        }
        if (setitimer(ITIMER_REAL, &val, NULL) == -1)
        {
            LOGE("setitimer fail.[%d]",errno);
            return;
        }
    }
    else     // UP
    {
        if(key_down)
        {
            // ms
            long time_used = (time->tv_sec - down_time.tv_sec) * 1000 + (time->tv_usec - down_time.tv_usec) / 1000;
            LOGI("Down time[%ld,%ld], Up time[%ld,%ld], time_used = %ld ms",down_time.tv_sec,down_time.tv_usec,
                 time->tv_sec,time->tv_usec,time_used);

            if(!power_long_press_timeout)
            {
                // Cancel alarm
                struct itimerval value;
                value.it_value.tv_sec = 0;
                value.it_value.tv_usec = 0;
                value.it_interval = value.it_value;
                setitimer(ITIMER_REAL, &value, NULL);

                if(time_used <= POWER_KEY_PRESS_TIMEOUT)
                {
                    screen_state_change(false);
                }
                else
                {
                    LOGI("Press timeout.");
                }
            }
            else
            {
                LOGI("Long Press timeout.");
            }
        }
        else
        {
            LOGI("UP key for screen on.");
        }

        key_down = false;
    }
#endif
}


int main(int argc, char *argv[])
{
	mbtk_log_init(0, NULL, "MBTK_KEY");
    // Open dev
    ev_init();

    int ret = 0;
    struct input_event ev;
    while(1)
    {
        ret = ev_get(&ev, -1);
        if(ret)
        {
            LOGW("ev_get() fail.");
            continue;
        }

        LOGI("ev:time[%ld,%ld],type:%d,code:%d,value:%d",ev.time.tv_sec,ev.time.tv_usec,
             ev.type,ev.code,ev.value);
        if(ev.type != EV_KEY)
        {
            LOGW("event type error.[%d]",ev.type);
            continue;
        }

        switch(ev.code)
        {
            case KEY_POWER:   // Power key
            {
                power_process(ev.value, &(ev.time));
                break;
            }
            default:
            {
                LOGD("Unknown KEY[%d]",ev.code);
                break;
            }
        }
    }
    return 0;
}

