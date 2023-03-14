# 编译
## config
    保存配置
    cp .config configs/asr1803_defconfig
## 设置编译器
    1. make menuconfig asr1803_defconfig

      Toolchain
      -> Toolchain type = External toolchain
      -> Toolchain = Custom toolchain //用户自己的交叉编译器
      -> Toolchain origin = Pre-installed toolchain //预装的编译器
      -> Toolchain path =/home/zhaixing/rv1109/prebuild/gcc
      -> Toolchain prefix = $(ARCH)-linux-gnueabihf //前缀
      -> External toolchain gcc version = 4.9.x
      -> External toolchain kernel headers series = 4.0.x
      -> External toolchain C library = glibc/eglibc
      -> [*] Toolchain has SSP support? (NEW) //选中
      -> [*] Toolchain has RPC support? (NEW) //选中
      -> [*] Toolchain has C++ support? //选中
      -> [*] Enable MMU support (NEW) //选中
## 下载第三方包速度慢
buildroot，重新加载第三方软件时，出现下载软件包太慢/失败
这时候我们选择所需下载的包的地址，自行下载。
下载完成后直接将包复制到buildroot/dl下面即可（不用解压缩）

## 单编模块
BuildRoot如何单独编译某一个包？
    如果修改了源码，在编译前运行 make < package >-dirclean
    如果只是修改output目录下的东西，编译前运行 make < package >-rebuild
example: make linux-rebuild
         make helloworld-dirclean

BuildRoot什么时候需要全部重新编译，什么时候只需要单独编译某一个包？
    目标体系结构修改时，需要全部重新编译
    编译工具链修改时，需要全部重新编译
    新增一个包无需全部重新编译，但是如果新增的是一个库，且别其他文件所引用，则需一起重新编译，或者全部重编。
    删除一个包的时候，需要全部重新编译，因为BuildRoot不会去删除编译产生的文件，这样会照成文件系统臃肿等问题，但是你没有必要马上重新编译，可以等到最后一起编译。
    文件系统框架修改时，除了overlay 之外，都需要全部重新编译。
## 添加自定义应用或lib
  如果有自己的应用或者lib 想放在rootfs里面，直接复制到 board/mobiletek/asr1803/rootfs-overlay
## busybox
make busybox-menuconfig可以配置busybox


mobiletek/kernel/scripts/dtc/dtc-lexer.lex.c_shipped
修改scripts/dtc目录下的dtc-lexer.lex.c_shipped文件中找到
YYLTYPE yyloc这一行，在640行，在之前面加上extern

static const struct kconf_id *kconf_id_lookup(register const char *str, register unsigned int len);
static const struct kconf_id *kconf_id_lookup(register const char *str, register size_t len);

    ~/work/opensdk_mbtk/mopen-tools/sysroots/linux64/arm-openwrt-linux/
make -j5 -C /home/sniper/work/buildroot-asr/output/build/uboot falcon_p401 CROSS_COMPILE="/home/sniper/work/buildroot-asr/output/host/bin/arm-openwrt-linux-" ARCH=arm
