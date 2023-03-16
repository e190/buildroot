#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define MBTK_UART_DEV  "/dev/ttyS1\0"
#define MBTK_UART_RECV_BUFFER_SIZE 1048
#define MBTK_UART_SEND_BUFFER_MAX 128

void mopen_uart_NonBlock(int fd, int cmd) {
    int flags;
    flags = fcntl(fd, F_GETFL, 0);

    if (cmd)
        flags |= O_NONBLOCK;
    else
        flags &= ~O_NONBLOCK;

    fcntl(fd, F_SETFL, flags);
}

/**
 * @brief      set_baudrate
 *
 * @details
 *
 * @param      baudrate: B9600,B115200 .....
 *
 * @return
 */
int set_baudrate(int fd, int baudrate) {
    struct termios options, oldtio;

    if (fcntl(fd, F_SETFL, 0) < 0) {
        printf("fcntl failed!\n");
        return -1;
    }

    if (tcgetattr(fd, &oldtio) != 0) {
        printf("setup serial error!\n");
        return -1;
    }

    /* Get the current options for the port... */
    tcgetattr(fd, &options);
    /* Set the baud rates to baudrate... */
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    tcsetattr(fd, TCSANOW, &options);

    if (0 != tcgetattr(fd, &options)) {
        printf("get options error!\n");
        return -1;
    }

    /*
     * 8bit Data,no partity,1 stop bit...
     */
    // 无效验       8位
    // options.c_cflag &= ~PARENB;//无奇偶校验
    // options.c_cflag &= ~CSTOPB;//停止位，1位
    // options.c_cflag &= ~CSIZE; //数据位的位掩码
    // options.c_cflag |= CS8;    //数据位，8位

    // 奇效验(Odd)  7位
    options.c_cflag |= (PARODD | PARENB);
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS7;
    options.c_iflag |= (INPCK | ISTRIP);

    // 偶效验(Even) 7位
    // options.c_cflag |= PARENB; /* Enable parity */
    // options.c_cflag &= ~PARODD; /* 转换为偶效验*/
    // options.c_iflag |= INPCK; /* Disnable parity checking */
    // options.c_cflag &= ~CSTOPB;
    // options.c_cflag &= ~CSIZE;
    // options.c_cflag |= CS7;

    /*
     * Set the new options for the port...
     */
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        printf("setup serial error!\n");
        return -1 ;
    }

    return 0 ;
}

//  baudrate: B9600,B115200 .....
int mopen_uart_open(char* dev, int baudrate) {
    int ret;
    int fd = 0;
    fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd < 0) {
        return -1;
    }

    printf("curent dev: %s, fd: %d \n", dev, fd);

    if (baudrate) {
        printf("set baudrate: %d \n", baudrate);
        ret = set_baudrate(fd, baudrate);

        if (-1 == ret) {
            close(fd);
            return -1;
        }
    } else {
        set_baudrate(fd, B9600);
    }

    return fd;
}

int mopen_uart_read(int fd, char* buf, unsigned int buf_len) {
    buf_len = (buf_len > MBTK_UART_RECV_BUFFER_SIZE ? MBTK_UART_RECV_BUFFER_SIZE : buf_len);
    return read(fd, buf, buf_len);
}

int mopen_uart_write(int fd, const char* buf, unsigned int buf_len) {
    size_t size;
    size_t size_to_wr;
    ssize_t size_written;

    if (MBTK_UART_SEND_BUFFER_MAX < buf_len) {
        return -1;
    }

    for (size = 0; size < buf_len;) {
        size_to_wr = buf_len - size;

        if ( size_to_wr > MBTK_UART_SEND_BUFFER_MAX)
            size_to_wr = MBTK_UART_SEND_BUFFER_MAX;

        size_written = write(fd, &buf[size], size_to_wr);

        if (size_written == -1) {
            return -1;
        }

        printf("send cmd: %s", &buf[size]);
        size += size_written;

        if (size_written != size_to_wr) {
            return size;
        }
    }

    return size;
}

int mopen_uart_close(int fd) {
    return close(fd);
}

int main(int argc, char* argv[]) {
    char buffer[MBTK_UART_RECV_BUFFER_SIZE];
    char msg0[128] = "UART Test begain Please Send message to me!\r\n";
    char msg1[128] = "Hi,I have got message that you send!\r\n";
    int uart_fd;
    int size = 0;
    printf("UART TEST\r\n");

	//  baudrate: B9600,B115200 .....
    uart_fd = mopen_uart_open(MBTK_UART_DEV, B115200);

    if (uart_fd == -1) {
        printf("open uart error!\n");
        return 0;
    }

    printf("MOpen_UART_NonBlock TEST\r\n");
    mopen_uart_NonBlock(uart_fd, 1);
    mopen_uart_write(uart_fd, msg0, strlen(msg0));

    do {
        memset(buffer, 0x0, sizeof(buffer));
        size = mopen_uart_read(uart_fd, buffer, MBTK_UART_RECV_BUFFER_SIZE);

        if (size > 0) {
            if (strncmp("quit", buffer, 4) == 0) {
                printf("#####UART Test end#######\n");
                size = mopen_uart_write(uart_fd, "Test end\r\n", strlen("Test end\r\n"));
                break;
            }

            printf("> read(uart)=%d:\n%s\n", size, buffer);
            size = mopen_uart_write(uart_fd, msg1, strlen(msg1));

            if (-1 == size) {
                printf("send_data error:%s\n", strerror(errno));
            }
        }
    } while (1);

    return 0;
}
