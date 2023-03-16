#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <math.h>

/**< \brief 根据实际情况修改，此处为unsigned short是565的屏，根据程序打印出的
    bits_per_pixel的值可以判断出输出格式是565还是888 */
// typedef unsigned int color_t;
typedef unsigned short color_t;
/**< \brief 定义每个像素点对应的位数，如果是565的屏则为16，如果是888的屏则为32 */
// #define BITS_PER_PIXEL    32
#define BITS_PER_PIXEL    16

static struct fb_var_screeninfo __g_vinfo;    /* 显示信息 */
color_t *__gp_frame;                    /* 虚拟屏幕首地址 */

#pragma pack(2)
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int DWORD;
typedef int LONG;

typedef struct tagBITMAPFILEHEADER
{
    WORD bfType;        // 位图文件的类型，必须为BM
    DWORD bfSize;       // 位图文件的大小，以字节为单位
    WORD bfReserved1;   // 位图文件保留字，必须为0
    WORD bfReserved2;   // 位图文件保留字，必须为0
    DWORD bfOffBits;    // 位图数据的起始位置，以相对于位图
                        // 文件头的偏移量表示，以字节为单位
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
    DWORD biSize; // 本结构所占用字节数
    LONG biWidth; // 位图的宽度，以像素为单位
    LONG biHeight; // 位图的高度，以像素为单位
    WORD biPlanes; // 目标设备的级别，必须为1
    WORD biBitCount;// 每个像素所需的位数，必须是1(双色),
         // 4(16色)，8(256色)或24(真彩色)之一
    DWORD biCompression; // 位图压缩类型，必须是 0(不压缩),
        // 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
    DWORD biSizeImage; // 位图的大小，以字节为单位
    LONG biXPelsPerMeter; // 位图水平分辨率，每米像素数
    LONG biYPelsPerMeter; // 位图垂直分辨率，每米像素数
    DWORD biClrUsed;// 位图实际使用的颜色表中的颜色数
    DWORD biClrImportant;// 位图显示过程中重要的颜色数
} BITMAPINFOHEADER;
#pragma pack(0)

typedef WORD (*bmp_xx_to_16)(char *);

//画点
void draw_point(int x, int y, color_t color)
{
    color_t *p = __gp_frame;

    p += __g_vinfo.xres * y + x;
    *p = color;
}

WORD bmp_24_to_16(char *input)
{
    /* 如果使用的bmp图片的颜色深度是24位，适用于888的屏，但如果一定要在565的屏
        上显示，则取红色的高5位，绿色的高6位和蓝色的高5位，拼成16位的数据
        进行显示。这样做并不是最好的办法，更好的方法是将需要丢失的部分数
        据进行进位或舍去。
        */
    WORD c;
    char b, g, r;
    r = *input >> 3;
    input++;
    g = *input >> 2;
    input++;
    b = *input >> 3;

    c = (b << 11) | (g << 5) | r;

    return c;
}

WORD bmp_16_to_16(char *input)
{
    WORD c;

    c = *input;
    input++;
    c = (c << 8) | *input;
    c = ((c >> 8) & 0x00ff) | ((c & 0x00ff) << 8);

    return c;
}

//功能：在指定坐标显示指定BPM24位图
//参数：（x , y）坐标
//        pic：24位BMP图像
void Show_BMP(int x , int y , const char *pic)
{
    int fd = 0;
    color_t c;
    BITMAPFILEHEADER filehead;
    BITMAPINFOHEADER infohead;
    int i,j;
    unsigned char pixel_byte;
    unsigned char *p = NULL , *p_data = NULL;
    int width_error = 0;
    short* t_data = NULL;
    bmp_xx_to_16 transform_func = NULL;
    int index = 0;

    printf("%s: %s\n", __FUNCTION__, pic);
    fd = open(pic , O_RDONLY);
    if(fd == -1) {
        printf("fail to open\n");
        return;
    }

    read(fd , &filehead , sizeof(filehead));
    read(fd , &infohead , sizeof(infohead));
    printf("bfType: 0x%x, bfSize: %d, bfOffBits: 0x%x\n", filehead.bfType, filehead.bfSize, filehead.bfOffBits);

    printf("biSize: %d, biWidth: %d, biHeight: %d\n", infohead.biSize, infohead.biWidth, infohead.biHeight);
    printf("biPlanes: %d, biBitCount: %d, biCompression: %d\n", infohead.biPlanes, infohead.biBitCount, infohead.biCompression);
    printf("biSizeImage: %d, biXPelsPerMeter: %d, biYPelsPerMeter: %d\n", infohead.biSizeImage, infohead.biXPelsPerMeter, infohead.biYPelsPerMeter);

    width_error = (4 - infohead.biWidth * 3 % 4) % 4;
    pixel_byte = infohead.biBitCount / 8;

    if (16 == infohead.biBitCount) {
        transform_func = bmp_16_to_16;
    } else if (24 == infohead.biBitCount) {
        transform_func = bmp_24_to_16;
    } else {
        printf("Not Suppurt %d bmp\n", infohead.biBitCount);
        close(fd);
        return;
    }

    t_data = malloc(__g_vinfo.xres_virtual * __g_vinfo.yres_virtual * __g_vinfo.bits_per_pixel / 8);

    if(t_data == NULL) {
        perror("fail to malloc");
    }

    p_data = malloc(infohead.biSizeImage);
    if(p_data == NULL) {
        perror("fail to malloc");
    }

    printf("biSizeImage:%d, width_error: %d\n", infohead.biSizeImage, width_error);
    read(fd , p_data , infohead.biSizeImage);
    p = p_data;

    int ret;
    char data[24] = {0};
    int debug_fd = open("/data/debug_fb", O_RDWR|O_CREAT|O_TRUNC, 0644);
    if (debug_fd < 0) {
        printf("debug_fb open error\n");
        return;
    }
    printf("height:%d, width:%d\n", infohead.biHeight, infohead.biWidth);
    for(j = infohead.biHeight - 1; j >= 0; j--) {
        for(i = 0; i < infohead.biWidth; i++) {
            c = transform_func(p);
            // c = *p;
            p += pixel_byte;
            // c = ((c >> 8) & 0x00ff) | ((c & 0x00ff) << 8);
            t_data[__g_vinfo.xres * (y + j) + (x + i)] = c;
            // draw_point(x + i, y + j, c);
            index++;

            sprintf(data, "index:%d, i:%d, j:%d\n", index, i, j);
            ret = write(debug_fd, data, strlen(data));
            if (ret < 0) {
                printf("%s write error\n", __FUNCTION__);
            }
        }
        p += width_error;
    }
    close(debug_fd);
    printf("%s: %d\n", __FUNCTION__, infohead.biHeight * infohead.biWidth * __g_vinfo.bits_per_pixel / 8);
    memcpy(__gp_frame, t_data,
           infohead.biHeight * infohead.biWidth * __g_vinfo.bits_per_pixel / 8);
    printf("%s: %d\n", __FUNCTION__, index);
    free(p_data);
    free(t_data);
    close(fd);
}

/**
 * \brief 填充整屏
 */
void full_screen (color_t color)
{
    int i;
    color_t *p = __gp_frame;

    for (i = 0; i < __g_vinfo.xres_virtual * __g_vinfo.yres_virtual; i++) {
        *p++ = color;
    }
}

/**
 * \brief 清屏
 */
void clear()
{
    full_screen(0);
}

/* framebuffer初始化 */
int framebuffer_init (void)
{
    int fd = 0;

    fd = open("/dev/fb0", O_RDWR);
    if (fd == -1) {
        perror("fail to open /dev/fb0\n");
        return -1;
    }

    /* 获取显示信息 */
    ioctl(fd, FBIOGET_VSCREENINFO, &__g_vinfo);                     /* 获取显示信息 */
    printf("bits_per_pixel = %d\n", __g_vinfo.bits_per_pixel);      /* 得到一个像素点对应的位数 */
    printf("xres_virtual = %d\n", __g_vinfo.xres_virtual);            /* 打印虚拟屏幕列数 */
    printf("yres_virtual = %d\n", __g_vinfo.yres_virtual);            /* 打印虚拟屏幕行数 */
    printf("xres = %d\n", __g_vinfo.xres);                            /* 打印屏幕列数 */
    printf("yres = %d\n", __g_vinfo.yres);                            /* 打印屏幕行数 */

    int len = __g_vinfo.xres_virtual * __g_vinfo.yres_virtual * __g_vinfo.bits_per_pixel / 8;    /* 映射区大小 */

    printf("fb size = %d\n", len);
    __gp_frame = mmap(NULL,                             /* 映射区的开始地址，为NULL表示由系统决定映射区的起始地址 */
                      len,
                      PROT_WRITE | PROT_READ,            /* 内存保护标志（可读可写） */
                      MAP_SHARED,                        /* 映射对象类型（与其他进程共享） */
                      fd,                                /* 有效的文件描述符 */
                      0);                                /* 被映射内容的偏移量 */
    if (__gp_frame == NULL) {
        perror("fail to mmap\n");
        return -1;
    }

    return fd;
}


int main(int argc, const char *argv[])
{
    int fd;

    if (argc < 2) {
        printf("%s \" img \"", argv[0]);
        exit(1);
    }

    fd = framebuffer_init();
    if (fd < 0) {
        printf("framebuffer_init error\n");
        return 0;
    }

    printf("framebuffer_init Success.\n");
    /* 清屏 */
    clear();

    printf("clear Success.\n");

    // full_screen(0xF800);  // 显示红色

    Show_BMP(0 , 0 , argv[1]);

    close(fd);

    return 0;
}
